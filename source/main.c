#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

#include <3ds.h>
#include <citro2d.h>

#define SOURCE "CARTS"
#define DEST "p8carts"

int cmpstringp(const void *p1, const void *p2) {
  return strcasecmp(*(char *const *)p1, *(char *const *)p2);
}

int getDir(struct dirList_t *dirList) {
  DIR *dp;
  struct dirent *ep;
  int filenum = 0;
  int dirnum = 0;
  char *wd = getcwd(NULL, 0);

  if (wd == NULL)
    goto out;

  for (int i = 0; i < dirList->dirNum; i++)
    free(dirList->directories[i]);
  for (int i = 0; i < dirList->fileNum; i++)
    free(dirList->files[i]);

  free(dirList->currentDir);

  if ((dirList->currentDir = strdup(wd)) == NULL)
    puts("Failure");

  if ((dp = opendir(wd)) == NULL)
    goto out;

  while ((ep = readdir(dp)) != NULL) {
    if (ep->d_type == DT_DIR) {
      dirList->directories =
          realloc(dirList->directories, (dirnum + 1) * sizeof(char *));
      if ((dirList->directories[dirnum] = strdup(ep->d_name)) == NULL)
        puts("Failure");
      dirnum++;
      continue;
    }

    dirList->files = realloc(dirList->files, (filenum + 1) * sizeof(char *));
    if ((dirList->files[filenum] = strdup(ep->d_name)) == NULL)
      puts("Failure");
    filenum++;
  }

  qsort(&dirList->files[0], filenum, sizeof(char *), cmpstringp);
  qsort(&dirList->directories[0], dirnum, sizeof(char *), cmpstringp);

  dirList->dirNum = dirnum;
  dirList->fileNum = filenum;

  if (closedir(dp) != 0)
    goto out;

out:
  free(wd);
  return filenum + dirnum;
}

int listDir(int from, int max, int select, struct dirList_t dirList) {
  int filenum = 0;
  int listed = 0;

  if (from == 0) {
    listed++;
    max--;
  }

  while (dirList.fileNum + dirList.dirNum > filenum) {
    filenum++;
    if (filenum <= from)
      continue;
    listed++;

    if (filenum == max + from)
      break;
  }

  return listed;
}

int getNumberFiles() {
  DIR *dp;
  struct dirent *ep;
  int ret = 0;

  if ((dp = opendir(".")) == NULL)
    goto err;
  while ((ep = readdir(dp)) != NULL)
    ret++;

  closedir(dp);

out:
  return ret;
err:
  ret = -1;
  goto out;
}

int removeFile(char *filename, struct dirList_t dirList) {
  char string[512];
  snprintf(string, sizeof(string), "%s/%s", dirList.currentDir, filename);
  if (remove(string) == 0)
    return 0;
  else
    return -1;
}

int copyFile(const char *src, const char *dst) {
  FILE *in = fopen(src, "rb");
  if (!in)
    return -1;

  FILE *out = fopen(dst, "wb");
  if (!out) {
    fclose(in);
    return -1;
  }

  char buffer[4096];
  u64 bytes;

  while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0)
    fwrite(buffer, 1, bytes, out);

  fclose(in);
  fclose(out);
  return 0;
}

int moveFile(char *destination, char *filename) {
	char string[512];
	char* source;

	char deststring[512];

	if (strcmp(destination, DEST)) source = SOURCE;
	else source = DEST;

	snprintf(string, sizeof string, "%s", filename);
	snprintf(deststring, sizeof deststring, "../%s/%s", destination, filename);

	if (copyFile(string, deststring) != 0) return -1;
	if (remove(string) != 0) return -1;
	return 0;
}

int main(void) {
  int fileMax;
  int fileNum = 0;
  int from = 0;
  struct dirList_t dirList = {0};
	char *notCurrDir;

  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  C3D_RenderTarget *bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

  C2D_TextBuf gTextBuf = C2D_TextBufNew(4096);
  C2D_Font gFont = C2D_FontLoadSystem(CFG_REGION_EUR);
  C2D_Text text;

  C2D_TextBuf tTextBuf = C2D_TextBufNew(4096);
  C2D_Text texttop;

  chdir(DEFAULT_DIR);
  chdir(SOURCE);

  if (getDir(&dirList) < 0) {
    puts("Unable to obtain directory information");
    goto err;
  }

  if (listDir(from, MAX_LIST, 0, dirList) < 0) {
    puts("Unable to list directory");
    goto err;
  }

  fileMax = dirList.fileNum - 1;

  u32 clrClear = C2D_Color32(0x23, 0x26, 0x39, 0xFF);
  u32 clrYel = C2D_Color32(0xEE, 0xD3, 0x9F, 0xFF);
  u32 clrGreen = C2D_Color32(0xA6, 0xDA, 0x94, 0xFF);
  u32 clrRed = C2D_Color32(0xEE, 0x98, 0xA0, 0xFF);
  u32 clrBlue = C2D_Color32(0x89, 0xAD, 0xF4, 0xFF);

  C2D_TextBufClear(tTextBuf);
  C2D_TextParse(&texttop, tTextBuf, "No Selection");
  C2D_TextOptimize(&texttop);

	bool inDest = false;

  while (aptMainLoop()) {

		notCurrDir = inDest ? SOURCE : DEST;

    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    static u64 mill = 0;
    if (kDown & KEY_START)
      break;

    if (kDown)
      mill = osGetTime();
    int visible = 20;

    // down
    if ((kDown & KEY_DOWN ||
         ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) &&
        fileNum < fileMax) {
      fileNum++;
      from = fileNum - (visible / 2);
      if (from < 0)
        from = 0;
      if (from > dirList.fileNum - visible)
        from = dirList.fileNum - visible;
      if (from < 0)
        from = 0;
    }

    // up
    if ((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) &&
        fileNum > 0) {
      fileNum--;
      from = fileNum - (visible / 2);
      if (from < 0)
        from = 0;
      if (from > dirList.fileNum - visible)
        from = dirList.fileNum - visible;
      if (from < 0)
        from = 0;
    }

		if (kDown & KEY_R) {
			char path[64];
			snprintf(path, sizeof path, "../%s", notCurrDir);
			if (chdir(path) == 0) {
				inDest = !inDest;

				getDir(&dirList);
				fileMax = dirList.fileNum - 1;

				if (fileNum > fileMax) fileNum = fileMax < 0 ? 0 : fileMax;
				from = 0;
			}
		}

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, clrClear);
    C2D_SceneBegin(top);

    C2D_DrawRectangle(400 / 2 - 50, 240 / 2 - 30, 0, 100, 60, clrBlue, clrBlue,
                      clrBlue, clrYel);

    if (kDown & KEY_A) {
      C2D_TextBufClear(tTextBuf);
      C2D_TextParse(&texttop, tTextBuf, dirList.files[fileNum]);
      C2D_TextOptimize(&texttop);
			moveFile(notCurrDir, dirList.files[fileNum]);
			getDir(&dirList);
			fileMax = dirList.fileNum -1;
			if (fileNum > fileMax) fileNum = fileMax < 0 ? 0 : fileMax;
    }
    C2D_DrawText(&texttop, C2D_AtBaseline | C2D_WithColor, 10.0f, 200.0f, 0.0f,
                 0.5f, 0.5f, clrYel);

    C2D_TargetClear(bot, clrClear);
    C2D_SceneBegin(bot);

    int start = from;
    int end = start + visible;
    if (end > dirList.fileNum)
      end = dirList.fileNum;
    float y = 10.0f;

    int cursorRow = visible / 2;

    for (int i = start; i < end; i++) {
      u32 col = i == fileNum ? clrYel : clrGreen;
      C2D_TextBufClear(gTextBuf);
      C2D_TextParse(&text, gTextBuf, dirList.files[i]);
      C2D_TextOptimize(&text);
      C2D_DrawText(&text, C2D_AtBaseline | C2D_WithColor, 10.0f, y, 0.0f, 0.5f,
                   0.5f, col);
      y += 12.0f;
    }

    C3D_FrameEnd(0);
  }

out:
  puts("Exiting..");
  C2D_FontFree(gFont);
  C2D_TextBufDelete(gTextBuf);
  C2D_Fini();
  C3D_Fini();
  gfxExit();
  return 0;

err:
  puts("A fatal error occured. Press START to exit.");
  while (true) {
    u32 kDown;
    hidScanInput();
    kDown = hidKeysDown();
    if (kDown & KEY_START)
      break;
  }
  goto out;
}
