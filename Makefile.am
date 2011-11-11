#SUBDIRS = src tests
EXTRA_DIST = bootstrap data bin_win32 vs2010

AM_CPPFLAGS = -I $(top_srcdir)/src -I /usr/include/SDL 

bin_PROGRAMS = AstroAttack AAEditor

common_src = \
    src/game/CameraController.cpp src/common/Component.cpp src/common/ComponentManager.cpp src/game/Configuration.cpp \
    src/common/DataLoader.cpp src/common/Event.cpp src/common/Font.cpp src/game/GameApp.cpp \
    src/common/GameState.cpp src/game/Gui.cpp src/game/Input.cpp \
    src/game/Logger.cpp src/game/OsMsgBox.cpp src/common/Physics.cpp \
    src/common/Renderer.cpp src/common/Sound.cpp src/common/Texture.cpp src/common/Vector2D.cpp \
    src/common/World.cpp \
    src/common/components/CompGravField.cpp src/common/components/CompPhysics.cpp \
    src/common/components/CompPlayerController.cpp src/common/components/CompPosition.cpp \
    src/common/components/CompShape.cpp src/common/components/CompTrigger_Conditions.cpp \
    src/common/components/CompTrigger.cpp src/common/components/CompTrigger_Effects.cpp \
    src/common/components/CompVisualAnimation.cpp \
    src/common/components/CompVisualMessage.cpp src/common/components/CompVisualTexture.cpp \
    src/game/states/EditorState.cpp \
    src/game/states/GameOverState.cpp src/game/states/MainMenuState.cpp \
    src/game/states/PlayingState.cpp src/game/states/SlideShowState.cpp \
    src/contrib/pugixml/pugixml.cpp
    
    
# === AstroAttack ===
AstroAttack_SOURCES = $(common_src) src/game/main.cpp


# === AAEditor ===
AAEditor_CXXFLAGS = `wx-config --cxxflags`

AAEditor_LDFLAGS = `wx-config --libs --gl-libs`

AAEditor_SOURCES = $(common_src) src/editor/EditorApp.cpp src/editor/EditorFrame.cpp \
    src/editor/gen/EditorFrameBase.cpp


# === AATest ===
if ENABLETEST
bin_PROGRAMS += AATest
AATest_SOURCES = $(common_src) tests/testmain.cpp \
                         tests/LevelLoadTest.cpp
AATest_LDFLAGS = `gtest-config --libs`
endif
