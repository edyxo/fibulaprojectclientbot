cmake_minimum_required(VERSION 3.16)
project(fibula-client CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

set(SOURCES
    src/animatedText.cpp
    src/animator.cpp
    src/automap.cpp
    src/config.cpp
    src/connection.cpp
    src/container.cpp
    src/creature.cpp
    src/distanceEffect.cpp
    src/effect.cpp
    src/engine.cpp
    src/game.cpp
    src/GUI/AdvancedGraphicsOptions.cpp
    src/GUI/AdminTools.cpp
    src/GUI/AutoEater.cpp
    src/GUI/AutoFisher.cpp
    src/GUI/AutoHealer.cpp
    src/GUI/AutoLogout.cpp
    src/GUI/AutoRuner.cpp
    src/GUI/AutoTarget.cpp
    src/GUI/BattleWindow.cpp
    src/GUI/BugReport.cpp
    src/GUI/Buttons.cpp
    src/GUI/Channels.cpp
    src/GUI/CharacterList.cpp
    src/GUI/Chat.cpp
    src/GUI/ConsoleOptions.cpp
    src/GUI/ContainerWindow.cpp
    src/GUI/DeathNotification.cpp
    src/GUI/Donate.cpp
    src/GUI/EnterGame.cpp
    src/GUI/ExitWarning.cpp
    src/GUI/GeneralOptions.cpp
    src/GUI/GraphicsOptions.cpp
    src/GUI/Health.cpp
    src/GUI/Help.cpp
    src/GUI/Hotkeys.cpp
    src/GUI/IgnoreList.cpp
    src/GUI/Info.cpp
    src/GUI/Inventory.cpp
    src/GUI/ItemMove.cpp
    src/GUI/itemUI.cpp
    src/GUI/Logout.cpp
    src/GUI/MagicWall.cpp
    src/GUI/MainWindow.cpp
    src/GUI/ManaTrainer.cpp
    src/GUI/Messagebox.cpp
    src/GUI/Minimap.cpp
    src/GUI/ModalDialog.cpp
    src/GUI/Motd.cpp
    src/GUI/Options.cpp
    src/GUI/Outfits.cpp
    src/GUI/ProtocolOptions.cpp
    src/GUI/Quests.cpp
    src/GUI/ReadWrite.cpp
    src/GUI/RuleViolations.cpp
    src/GUI/ServerBrowser.cpp
    src/GUI/ShopWindow.cpp
    src/GUI/SkillsWindow.cpp
    src/GUI/SpellsWindow.cpp
    src/GUI/TradeWindow.cpp
    src/GUI/TutorialHint.cpp
    src/GUI/VipWindow.cpp
    src/GUI_Elements/GUI_Button.cpp
    src/GUI_Elements/GUI_CharacterView.cpp
    src/GUI_Elements/GUI_CheckBox.cpp
    src/GUI_Elements/GUI_Console.cpp
    src/GUI_Elements/GUI_Container.cpp
    src/GUI_Elements/GUI_Content.cpp
    src/GUI_Elements/GUI_ContextMenu.cpp
    src/GUI_Elements/GUI_Description.cpp
    src/GUI_Elements/GUI_Element.cpp
    src/GUI_Elements/GUI_Flash.cpp
    src/GUI_Elements/GUI_Grouper.cpp
    src/GUI_Elements/GUI_Icon.cpp
    src/GUI_Elements/GUI_Label.cpp
    src/GUI_Elements/GUI_ListBox.cpp
    src/GUI_Elements/GUI_Log.cpp
    src/GUI_Elements/GUI_MultiTextBox.cpp
    src/GUI_Elements/GUI_Panel.cpp
    src/GUI_Elements/GUI_PanelWindow.cpp
    src/GUI_Elements/GUI_ScrollBar.cpp
    src/GUI_Elements/GUI_Separator.cpp
    src/GUI_Elements/GUI_StaticImage.cpp
    src/GUI_Elements/GUI_TextBox.cpp
    src/GUI_Elements/GUI_Window.cpp
    src/http.cpp
    src/inputMessage.cpp
    src/item.cpp
    src/json/json.cpp
    src/light.cpp
    src/LuaRuntime.cpp
    src/main.cpp
    src/map.cpp
    src/outputMessage.cpp
    src/protocol.cpp
    src/protocolgame.cpp
    src/protocollogin.cpp
    src/protocolloginHttp.cpp
    src/rsa.cpp
    src/screenText.cpp
    src/softwareDrawning.cpp
    src/spriteManager.cpp
    src/staticText.cpp
    # D3D files compile as empty TUs on Linux (guarded by SDL_VIDEO_RENDER_D3D*)
    src/surfaceDirect3D11.cpp
    src/surfaceDirect3D9.cpp
    src/surfaceDirectDraw.cpp
    src/surfaceOpengl.cpp
    src/surfaceOpenglCore.cpp
    src/surfaceOpengles.cpp
    src/surfaceOpengles2.cpp
    src/surfaceSoftware.cpp
    src/surfaceVulkan.cpp
    src/thing.cpp
    src/thingManager.cpp
    src/tile.cpp
    src/util.cpp
    # LZMA — exclude Threads.c and LzFindMt.c (Windows-only threading)
    src/lzma/Alloc.c
    src/lzma/LzFind.c
    src/lzma/LzmaDec.c
    src/lzma/LzmaEnc.c
    src/lzma/LzmaLib.c
    # Lua 5.4 (single-file build)
    src/third_party/lua54/onelua.c
)

add_executable(fibula-client ${SOURCES})

target_include_directories(fibula-client PRIVATE
    src/
    # SDL2_INCLUDE_DIRS points to /usr/include/SDL2 but sources use
    # #include <SDL2/SDL.h>, which resolves via the default /usr/include path.
    # Do NOT add SDL2_INCLUDE_DIRS here — it would cause a double-prefix path.
)

target_compile_definitions(fibula-client PRIVATE
    _7ZIP_ST          # Disable LZMA multithreading (Windows-only)
)

target_compile_options(fibula-client PRIVATE
    # SDL2 extra compiler flags (e.g., -pthread, -D_REENTRANT)
    ${SDL2_CFLAGS_OTHER}
    -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable
    # SIMD flags required by softwareDrawning.cpp / util.cpp SSSE3/SSE4/AVX2/FMA code paths
    -msse2 -mssse3 -msse4.1 -msse4.2 -mfma -mavx -mavx2
)

# Suppress warnings in third-party C code
set_source_files_properties(
    src/lzma/Alloc.c
    src/lzma/LzFind.c
    src/lzma/LzmaDec.c
    src/lzma/LzmaEnc.c
    src/lzma/LzmaLib.c
    PROPERTIES COMPILE_FLAGS "-w"
)
# onelua.c is a single-file Lua build — MAKE_LIB suppresses its own main()
set_source_files_properties(
    src/third_party/lua54/onelua.c
    PROPERTIES COMPILE_FLAGS "-w -DMAKE_LIB"
)

target_link_directories(fibula-client PRIVATE ${SDL2_LIBRARY_DIRS})

target_link_libraries(fibula-client PRIVATE
    ${SDL2_LIBRARIES}
    CURL::libcurl
    ZLIB::ZLIB
    OpenGL::GL
    Threads::Threads
    m
    dl
)

install(TARGETS fibula-client RUNTIME DESTINATION bin)
