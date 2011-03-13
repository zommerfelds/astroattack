#SUBDIRS = src tests
EXTRA_DIST = bootstrap data bin_win32 vs2010

bin_PROGRAMS = AstroAttack

common_src = src/CameraController.cpp src/Component.cpp src/Configuration.cpp \
    src/DataLoader.cpp src/Entity.cpp src/Event.cpp src/Font.cpp src/GameApp.cpp \
    src/GameStates.cpp src/Gui.cpp src/Input.cpp \
    src/Logger.cpp src/OsMsgBox.cpp src/Physics.cpp \
    src/Renderer.cpp src/Sound.cpp src/Texture.cpp src/Vector2D.cpp \
    src/World.cpp \
    src/components/CompGravField.cpp src/components/CompPhysics.cpp \
    src/components/CompPlayerController.cpp src/components/CompPosition.cpp \
    src/components/CompShape.cpp src/components/CompTrigger_Conditions.cpp \
    src/components/CompTrigger.cpp src/components/CompTrigger_Effects.cpp \
    src/components/CompVisualAnimation.cpp \
    src/components/CompVisualMessage.cpp src/components/CompVisualTexture.cpp \
    src/states/EditorState.cpp \
    src/states/GameOverState.cpp src/states/MainMenuState.cpp \
    src/states/PlayingState.cpp src/states/SlideShowState.cpp \
    src/contrib/pugixml/pugixml.cpp
    
AstroAttack_SOURCES = $(common_src) src/main.cpp
    
AM_CXXFLAGS = -I /usr/include/SDL -I $(top_srcdir)/src

if ENABLETEST
bin_PROGRAMS += AstroAttackTest
AstroAttackTest_SOURCES = $(common_src) tests/testmain.cpp \
                         tests/LevelLoadTest.cpp
AstroAttackTest_LDFLAGS = `gtest-config --libs`
endif

