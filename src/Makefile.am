bin_PROGRAMS = AstroAttack

#AM_CXXFLAGS = -Wall

INCLUDES = -I"contrib/Box2D/Include"
LDADD = $(top_builddir)/src/contrib/Box2D/Library/libbox2d.a

AstroAttack_SOURCES = Camera.cpp Component.cpp Configuration.cpp \
	Entity.cpp EventManager.cpp Font.cpp GameApp.cpp \
	GameStates.cpp GUI.cpp Init.cpp Input.cpp \
	Logger.cpp main.cpp OsMsgBox.cpp Physics.cpp \
	Renderer.cpp Sound.cpp Texture.cpp Vector2D.cpp \
	World.cpp XmlLoader.cpp \
	components/CompCollectable.cpp components/CompGravField.cpp \
	components/CompPhysics.cpp components/CompPlayerController.cpp \
	components/CompPosition.cpp components/CompTrigger_Conditions.cpp \
	components/CompTrigger.cpp components/CompTrigger_Effects.cpp \
	components/CompVisualAnimation.cpp components/CompVisual.cpp \
	components/CompVisualMessage.cpp components/CompVisualTexture.cpp \
	contrib/tinyxml/tinystr.cpp contrib/tinyxml/tinyxml.cpp \
	contrib/tinyxml/tinyxmlerror.cpp contrib/tinyxml/tinyxmlparser.cpp \
	states/EditorState.cpp \
	states/GameOverState.cpp states/MainMenuState.cpp \
	states/PlayingState.cpp states/SlideShowState.cpp
#AstroAttack_INCLUDES = -I"contrib/Box2D/Include"
#AstroAttack_LDADD = -L"contrib/Box2D/Library" -lbox2d
AstroAttack_LDADD = -lBox2D

