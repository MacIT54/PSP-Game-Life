#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psputility.h>
#include <psploadexec.h>

PSP_MODULE_INFO("main", 0, 1, 0);

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int callbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();
    return 0;
}

void setupCallbacks()
{
    int thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, NULL);

    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);
}

void delayMicroseconds(unsigned int microseconds)
{
    sceKernelDelayThread(microseconds / 1000);
}

auto main() -> int
{

    setupCallbacks();
    pspDebugScreenInit();
    pspDebugScreenPuts("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\tWelcome to the game - 'Life' version 1.0 \n\n\t\t\t\tPress 'x' to start the game\n\n\t\t\t\tTo finish the game press 'o'");

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SceCtrlData ctrlData;

    SceKernelUtilsMt19937Context ctx;

    const int gridsize = 35;
    const int gridsize2 = 68;

    unsigned int seed = (unsigned int)sceKernelGetSystemTimeLow();
    sceKernelUtilsMt19937Init(&ctx, seed);

    unsigned int randomNumber;
    unsigned int binaryNumber;
    unsigned int nanoseconds;

    bool grid[gridsize + 1][gridsize2 + 1] = {};
    bool grid2[gridsize + 1][gridsize2 + 1] = {};

    for (int i = 1; i < gridsize; i++)
    {
        for (int j = 1; j < gridsize2; j++)
        {
            randomNumber = sceKernelUtilsMt19937UInt(&ctx);
            binaryNumber = randomNumber % 2;
            if (binaryNumber == 0)
                grid[i][j] = false;
            if (binaryNumber == 1)
                grid[i][j] = true;
        }
    }

    while (1)
    {
        sceCtrlReadBufferPositive(&ctrlData, 1);

        if (ctrlData.Buttons & PSP_CTRL_CROSS)
        {
            do
            {
                pspDebugScreenClear();
                for (int a = 1; a < gridsize; a++)
                {
                    for (int b = 1; b < gridsize2; b++)
                    {
                        if (grid[a][b] == true)
                            pspDebugScreenPrintf("#");
                        else
                            pspDebugScreenPrintf(" ");
                        if (b == gridsize2 - 1)
                            pspDebugScreenPrintf("\n");
                    }
                }
                for (int a = 0; a < gridsize; a++)
                    for (int b = 0; b < gridsize2; b++)
                        grid2[a][b] = grid[a][b];
                for (int a = 1; a < gridsize; a++)
                {
                    for (int b = 1; b < gridsize2; b++)
                    {
                        int life = 0;
                        for (int c = -1; c < 2; c++)
                        {
                            for (int d = -1; d < 2; d++)
                            {
                                if (c == 0 && d == 0)
                                    continue;
                                if (grid2[a + c][b + d])
                                    life++;
                            }
                        }
                        bool isAlive = false;
                        if (life == 3)
                            isAlive = true;
                        else if (life == 2 && grid[a][b])
                            isAlive = true;
                        grid[a][b] = isAlive;
                    }
                }
                sceCtrlReadBufferPositive(&ctrlData, 1);

                if (ctrlData.Buttons & PSP_CTRL_CIRCLE)
                    break;
                nanoseconds = 150000;
                delayMicroseconds(1 * nanoseconds);
            } while (1);
        }
        sceCtrlReadBufferPositive(&ctrlData, 1);
        if (ctrlData.Buttons & PSP_CTRL_CIRCLE)
            break;
    }
    pspDebugScreenClear();
    pspDebugScreenPuts("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\tGame over\n");
}