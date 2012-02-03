#SUBDIRS = src tests
EXTRA_DIST = bootstrap data bin_win32 vs2010

AM_CPPFLAGS = -I $(top_srcdir)/src -I /usr/include/SDL

bin_PROGRAMS = AstroAttack AAEditor

pugi_src = \
    src/contrib/pugixml/pugixml.cpp

common_src = \
    src/common/CameraController.cpp \
    src/common/Component.cpp \
    src/common/ComponentManager.cpp \
    src/common/DataLoader.cpp \
    src/common/Event.cpp \
    src/common/Font.cpp \
    src/common/Logger.cpp \
    src/common/Physics.cpp \
    src/common/Renderer.cpp \
    src/common/Sound.cpp \
    src/common/Texture.cpp \
    src/common/TriggerSystem.cpp \
    src/common/Vector2D.cpp \
    src/common/components/CompGravField.cpp \
    src/common/components/CompPhysics.cpp \
    src/common/components/CompPlayerController.cpp \
    src/common/components/CompPosition.cpp \
    src/common/components/CompShape.cpp \
    src/common/components/CompTrigger_Conditions.cpp \
    src/common/components/CompTrigger.cpp \
    src/common/components/CompTrigger_Effects.cpp \
    src/common/components/CompVariable.cpp \
    src/common/components/CompVisualAnimation.cpp \
    src/common/components/CompVisualMessage.cpp \
    src/common/components/CompVisualTexture.cpp
    
game_src = \
    src/game/main.cpp \
    src/game/Configuration.cpp \
    src/game/GameApp.cpp \
    src/game/GameState.cpp \
    src/game/Gui.cpp \
    src/game/Input.cpp \
    src/game/InputCameraController.cpp \
    src/game/OsMsgBox.cpp \
    src/game/PlayerController.cpp \
    src/game/states/GameOverState.cpp src/game/states/MainMenuState.cpp \
    src/game/states/PlayingState.cpp src/game/states/SlideShowState.cpp

editor_src = \
    src/editor/Editor.cpp \
    src/editor/EditorApp.cpp \
    src/editor/EditorFrame.cpp \
    src/editor/GlCanvasController.cpp \
    src/editor/gen/EditorGuiBase.cpp
    
test_src = \
    tests/testmain.cpp \
    tests/LevelLoadTest.cpp
    
# === AstroAttack ===
AstroAttack_SOURCES = $(common_src) $(game_src) $(pugi_src)

# === AAEditor ===
AAEditor_CXXFLAGS = @WX_CXXFLAGS@
AAEditor_LDADD = @WX_LIBS@ @WX_GL_LIBS@
AAEditor_SOURCES = $(common_src) $(editor_src)

# === AATest ===
if ENABLETEST
bin_PROGRAMS += AATest
AATest_SOURCES = $(common_src) $(test_src)
#AATest_LDADD = `gtest-config --libs`
AATest_LDADD = -lpthread -lgtest
endif
