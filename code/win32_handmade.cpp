#include <windows.h>
#include "handmade.h"
// TODO audio!
// TODO input playback is doesn't work
global bool appIsRunning;


struct Backbuffer
{
    BITMAPINFO bitmapInfo;
    void *memory;
    s32 width;
    s32 height;
    s32 stride; 
};

global Backbuffer backbuffer;


struct windowDimensionResult
{
    s32 width;
    s32 height;
};


struct win32_file_content
{
    void *memory;
    u32 size;
};

struct win32_state
{
    HANDLE recordFile;
    HANDLE playbackFile;
    
    s32 recordIndex;
    s32 playbackIndex;
    
    u64 totalSize;
    void *gameMemoryBlock;
};

win32_file_content readEntireFile(char *filename)
{
    win32_file_content result = {};
    
    HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER sizeOfFile;
        GetFileSizeEx(fileHandle,&sizeOfFile);
        result.size = (u32)sizeOfFile.QuadPart;
        result.memory = VirtualAlloc(0, result.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        u32 bytesToRead = result.size;
        DWORD bytesRidden;
        if(ReadFile(fileHandle, result.memory,bytesToRead, &bytesRidden,0))
        {
            
        }
        else
        {
            VirtualFree(result.memory,0,MEM_RELEASE);
        }
    }
    
    return result; 
}

bool writeEntireFile(win32_file_content *file,char *filename)
{
    bool result = false;
    
    HANDLE fileHandle = CreateFileA(filename,GENERIC_WRITE,FILE_SHARE_WRITE,0,CREATE_ALWAYS,0,0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesToWrite = file->size;
        DWORD bytesWritten = 0;
        if(WriteFile(fileHandle, file->memory, bytesToWrite, &bytesWritten,0))
        {
            result = (bytesToWrite == bytesWritten);
            CloseHandle(fileHandle);
        }
        else
        {
            // TODO (logging) 
        }
    }
    else
    {
        // TODO (logging) 
    }
    
    return result;
}

internal windowDimensionResult getWindowDimension(HWND window)
{
    windowDimensionResult result = {};
    RECT rect = {};
    
    
    GetWindowRect(window, &rect);
    result.width  = rect.right - rect.left;
    result.height = rect.bottom - rect.top;
    
    
    return result;
}

inline FILETIME getLastWriteTime(char *fileName)
{
    FILETIME lastWriteTime = {};
    
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;// = {};
    
    GetFileAttributesExA(fileName,
                         GetFileExInfoStandard,
                         &fileAttributes
                         );
    
    
    lastWriteTime = fileAttributes.ftLastWriteTime;
    
    return lastWriteTime;
}

struct win32_game_code
{
    HMODULE loadGameCodeDLL;
    game_update_and_render *updateAndRender;
    FILETIME lastWriteTime;
    
    bool isValid;
};

//NOTE loadGameCode
internal win32_game_code loadGameCode(char *dllFileName)
{
    win32_game_code result = {};
    
    
    result.lastWriteTime = getLastWriteTime(dllFileName);
    s32 num = CopyFile(dllFileName,
                       "handmadeTemp.dll",
                       FALSE
                       );
    
    result.loadGameCodeDLL = LoadLibraryA("handmadeTemp.dll");
    
    if(result.loadGameCodeDLL)
    {
        result.updateAndRender = (game_update_and_render*)GetProcAddress(result.loadGameCodeDLL, "gameUpdateAndRender");
        
        if(result.updateAndRender)
        {
            result.isValid = true;
        }
    }
    
    if(!result.isValid)
    {
        result.updateAndRender = game_update_and_render_stub;
        result.loadGameCodeDLL = LoadLibraryA("handmade.dll"); 
    }
    
    return result;
}

//NOTE unloadGameCode
internal void unloadGameCode(win32_game_code gameCode)
{
    if(gameCode.loadGameCodeDLL)
    {
        FreeLibrary(
                    gameCode.loadGameCodeDLL
                    );
    }
    gameCode.loadGameCodeDLL = 0;
    gameCode.isValid = false;
    gameCode.updateAndRender = game_update_and_render_stub;
}

internal void createBackbuffer(Backbuffer *backbuffer, s32 width, s32 height)
{
    if(backbuffer->memory)
    {
        VirtualFree(backbuffer->memory,0,MEM_RELEASE);
    }
    
    backbuffer->width  = width;
    backbuffer->height = height;
    backbuffer->bitmapInfo = {};
    
    backbuffer->bitmapInfo.bmiHeader.biSize = sizeof(backbuffer->bitmapInfo.bmiHeader);
    backbuffer->bitmapInfo.bmiHeader.biWidth = backbuffer->width;
    //NOTE if -biHeight up-bottom buffer + reverse
    backbuffer->bitmapInfo.bmiHeader.biHeight = -backbuffer->height;
    backbuffer->bitmapInfo.bmiHeader.biPlanes = 1;
    backbuffer->bitmapInfo.bmiHeader.biBitCount = 32; //NOTE(shvayko): 
    backbuffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    u32 bitmapTotalSize = (backbuffer->width * backbuffer->height) * 4;
    backbuffer->stride = backbuffer->width * 4;
    backbuffer->memory  = VirtualAlloc(0, bitmapTotalSize, 
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    
    
    
}

inline LARGE_INTEGER getClockValue()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter;
}

inline f32 getDifferenceTime(LARGE_INTEGER endTime,LARGE_INTEGER beginTime)
{
    f32 result;
    
    result = (f32)(endTime.QuadPart - beginTime.QuadPart);
    
    return result;
}

internal void stretchDIbits(Backbuffer *backbuffer, HDC deviceContext, s32 width, s32 height)
{
    
    s32 upperLeftXOffset = 15;
    s32 upperLeftYOffset = 15;
    
    PatBlt(deviceContext,0,0,width,upperLeftYOffset, BLACKNESS);
    PatBlt(deviceContext,0,upperLeftYOffset+backbuffer->height,width,height, BLACKNESS);
    PatBlt(deviceContext,0,0,upperLeftXOffset,height, BLACKNESS);
    PatBlt(deviceContext,0,upperLeftXOffset+backbuffer->width,width,height, BLACKNESS);
    
    
    StretchDIBits(
                  deviceContext,
                  upperLeftXOffset,upperLeftYOffset,
                  backbuffer->width, backbuffer->height,
                  0,0, backbuffer->width, backbuffer->height,
                  backbuffer->memory,
                  &backbuffer->bitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY
                  );
}
// TODO(shvayko): message proccessing in main loop
LRESULT CALLBACK mainWindowProc(HWND    window,
                                UINT    message,
                                WPARAM  wParam,
                                LPARAM  lParam
                                )
{
    LRESULT result = 0;
    
    
    switch(message)
    {
        case WM_QUIT:
        {
            appIsRunning = false;
            OutputDebugStringA("WM_QUIT\n");
        }break;
        case WM_DESTROY:
        {
            appIsRunning = false;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC deviceContext = BeginPaint(window, &ps);
            windowDimensionResult winDim = getWindowDimension(window);
            stretchDIbits(&backbuffer,deviceContext, winDim.width,winDim.height);
            EndPaint(window, &ps);
            OutputDebugStringA("WM_PAINT\n");
        }break;
        case WM_CREATE:
        {
            OutputDebugStringA("WM_CREATE\n");
        }break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            OutputDebugStringA("Should not proccess that! ERROR!\n");
        }break;
        default:
        {
            result =  DefWindowProc(window, message, wParam, lParam);
        }break;
    }
    
    
    return result;
}

internal void win32KeyboardProccessing(game_button_state *newState, bool isDown)
{
    assert(newState->endedDown != isDown);
    
    
    newState->endedDown = isDown;
    ++newState->halfTransitionCount;
    
}

internal void win32EndRecordInput(win32_state *winState)
{
    CloseHandle(winState->recordFile);
    winState->recordIndex = 0;
}

internal void win32BeginRecordInput(win32_state *winState, s32 inputPlayingIndex)
{
    winState->recordIndex = inputPlayingIndex;
    
    char *filename = "playback.hmi";
    
    winState->recordFile= CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE,0,CREATE_ALWAYS,0,0);
    
    DWORD bytesToRead = (DWORD)winState->totalSize;
    DWORD bytesRead;
    
    ReadFile(winState->playbackFile, winState->gameMemoryBlock,bytesToRead,&bytesRead,0);
}

internal void win32EndInputPlayback(win32_state *winState)
{
    CloseHandle(winState->playbackFile);
    winState->playbackIndex = 0;
}

internal void win32BeginInputPlayBack(win32_state *winState, s32 inputPlayingIndex)
{
    winState->playbackIndex = inputPlayingIndex;
    char *filename = "playback.hmi";
    winState->playbackFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,0,CREATE_ALWAYS,0,0);
}

internal void win32RecordInput(win32_state *winState, game_input *newInput)
{
    DWORD bytesWritten;
    WriteFile(winState->recordFile, newInput,sizeof(*newInput), &bytesWritten,0);
}

internal void win32PlaybackInput(win32_state *winState, game_input *newInput)
{
    DWORD bytesRead = 0;
    ReadFile(winState->playbackFile, newInput, sizeof(*newInput),&bytesRead,0);
    if( bytesRead == 0)
    {
        s32 playingIndex = winState->playbackIndex;
        win32EndInputPlayback(winState);
        win32BeginInputPlayBack(winState, playingIndex);
        ReadFile(winState->playbackFile, newInput, sizeof(*newInput),&bytesRead,0);
    }
}


internal void  win32KeyboardMessageProccessing(win32_state *winState,game_controller_input *keyboardController)
{
    MSG message;
    
    while(PeekMessageA(&message,0,0,0,PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                bool isDown = (message.lParam & (1 << 31)) == 0;
                bool wasDown = (message.lParam & (1 << 30)) != 0;
                if(isDown != wasDown)
                {
                    switch(message.wParam)
                    {
                        case 'W':
                        {
                            win32KeyboardProccessing(&keyboardController->buttonUp, isDown);
                        }break;
                        case 'A':
                        {
                            win32KeyboardProccessing(&keyboardController->buttonLeft, isDown);
                            
                        }break;
                        case 'S':
                        {
                            win32KeyboardProccessing(&keyboardController->buttonDown, isDown);
                        }break;
                        case 'D':
                        {
                            win32KeyboardProccessing(&keyboardController->buttonRight, isDown);
                        }break;
                        case VK_RETURN:
                        {
                            win32KeyboardProccessing(&keyboardController->buttonEnter, isDown);
                        }break;
                        case 'L':
                        {
                            if(isDown)
                            {
                                if(winState->recordIndex == 0)
                                {
                                    win32BeginRecordInput(winState, 1);
                                }
                                else
                                {
                                    win32EndRecordInput(winState);
                                    win32BeginInputPlayBack(winState,1);
                                }
                                
                            }break;
                            
                            default:
                            {
                                // TODO default?
                            }break;
                        }
                    }
                };
            }break;
            default:
            {
                TranslateMessage(&message); 
                DispatchMessage(&message); 
            }break;
        }
    }
}

int  WinMain(
             HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR     lpCmdLine,
             int       nShowCmd
             )
{
    createBackbuffer(&backbuffer, 980,720);
    WNDCLASS wc = {};
    
    LARGE_INTEGER performanceFrequency;
    QueryPerformanceFrequency(&performanceFrequency);
    s64 performanceFreq = performanceFrequency.QuadPart;
    
    char *className = "myClassName";
    // 
    wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    wc.lpfnWndProc = mainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    
    RegisterClassA(&wc);
    
    char *windowName = "handmade";
    u32 windowStyles = 0;//WS_EX_TOPMOST;
    HWND window = CreateWindowExA(windowStyles,wc.lpszClassName, windowName,
                                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT,CW_USEDEFAULT, 0,0,hInstance,0);
    
    ShowWindow(window, nShowCmd);
    // NOTE memory
    
    win32_state winState = {};
    game_memory gameMemory = {};
    
    LPVOID baseAddress = (LPVOID)terabytes(2);
    
    gameMemory.permanentStorageSize = megabytes(64LL);
    gameMemory.transientStorageSize = gigabytes(1);
    winState.totalSize= gameMemory.permanentStorageSize + gameMemory.transientStorageSize;
    winState.gameMemoryBlock = VirtualAlloc(baseAddress, winState.totalSize,
                                            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    gameMemory.permanentStorage = winState.gameMemoryBlock;
    
    gameMemory.transientStorage = VirtualAlloc(0, gameMemory.transientStorageSize,
                                               MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    
    
    
    
    // TODO(shvayko): my hotreloading doesn't work because my loop too faster and
    // time for reload dll is the same! fixed-frame rate should fix this bug
    char *sourceDLLname = "handmade.dll";
    appIsRunning = true;
    win32_game_code game = loadGameCode(sourceDLLname);
    f32 targetSecondsPerFrame = 1.f / 60.f;
    s64 differenceTime = 0;
    HDC deviceContext = GetDC(window);
    game_input input[2] = {};
    game_input *newInput = &input[0];
    game_input *oldInput = &input[1];
    game_button_state buttonState;
    LARGE_INTEGER lastTime = getClockValue();
    int frameCounter = 0;
    while(appIsRunning)
    {
        newInput->dtForFrame = targetSecondsPerFrame;
        FILETIME DLLnewWriteTime = getLastWriteTime(sourceDLLname);
        
        if(CompareFileTime(&DLLnewWriteTime, &game.lastWriteTime) != 0 )
            
        {
            char *filename = "pdb.lock";
            WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
            // NOTE(shvayko): check for pdb.lock file. If it there so skip reloading!
            bool reload = GetFileAttributesExA(filename, GetFileExInfoStandard,&fileAttributes);
            if(!reload)
            {
                unloadGameCode(game);
                game = loadGameCode(sourceDLLname);
            }
            
        }
        
        
        game_controller_input *oldKeyboardController = &oldInput->controllers[0];
        game_controller_input *newKeyboardController = &newInput->controllers[0];
        game_controller_input zeroController = {};
        *newKeyboardController = zeroController;
        
        for(s32 buttonIndex = 0; buttonIndex < 5; buttonIndex++)
        {
            newKeyboardController->buttons[buttonIndex].endedDown = oldKeyboardController->buttons[buttonIndex].endedDown;
        }
        
        win32KeyboardMessageProccessing(&winState,newKeyboardController);
        
        if(winState.recordIndex)
        {
            win32RecordInput(&winState,newInput);
        }
        
        if(winState.playbackIndex)
        {
            win32PlaybackInput(&winState, newInput);
        }
        
        
        
        windowDimensionResult winDim = getWindowDimension(window);
        stretchDIbits(&backbuffer, deviceContext, winDim.width, winDim.height);
        //renderSomething(&backbuffer,xOffset);
        GameBackbuffer gameBackbuffer;
        gameBackbuffer.memory = backbuffer.memory;
        gameBackbuffer.width = backbuffer.width;
        gameBackbuffer.height = backbuffer.height;
        gameBackbuffer.stride = backbuffer.stride;
        
        
        game.updateAndRender(&gameMemory,&gameBackbuffer,newInput);
        LARGE_INTEGER currentTime = getClockValue();
        
        
        
        f32 delta = getDifferenceTime(currentTime,lastTime)  / (f32)performanceFreq;
        f32 secondsElapsedSoFar = delta;
        while(secondsElapsedSoFar < targetSecondsPerFrame)
        {
            LARGE_INTEGER currentTime = getClockValue();
            secondsElapsedSoFar = getDifferenceTime(currentTime, lastTime) / (f32)performanceFreq;
        }
        
        
        QueryPerformanceCounter(&currentTime);
        // TODO(shvayko): this show not correct numbers
        s32 miliseconds = (s32)(secondsElapsedSoFar * 1000.f);
        s32 FPS = (s32)(1.f/secondsElapsedSoFar);
#if 0
        char outputTimingBuffer[255];
        wsprintf(outputTimingBuffer, "ms:%d FPS:%d \n", miliseconds, FPS);
        OutputDebugStringA(outputTimingBuffer);
#endif
        game_input *temp = newInput;
        newInput = oldInput;
        oldInput = temp;
        lastTime = currentTime;
        
    }
    
    
    
    return 0;
}