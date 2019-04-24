#include "GUIManager.h"
#include "..\..\Core\Window.h"
#include "GUIText.h"
#include "..\..\Event\ApplicationEvents.h"
#include "..\..\Core\Core.h"
#include <thread>



GUIManager& GUIManager::Instance()
{
	static GUIManager instance;
	return instance;
}

GUIManager::~GUIManager()
{}

void GUIManager::Initialize()
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(Window::Instance().window, false);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Load fonts
	ImFont* defaultF = ImGui::GetIO().Fonts->AddFontDefault();

	ImFont* arcade = ImGui::GetIO().Fonts->AddFontFromFileTTF("Assets\\Fonts\\arcadeclassic\\ARCADECLASSIC.TTF", 25);
	
	allFonts["defaultFont"] = arcade;

	allFonts["arcadeFont"] = arcade;

	EventDispatcher::Instance().SubscribeCallback<SceneChangedEvent>([this](Event* e){

		DeleteGUIObjects(0);
		return 0;
	});
}



void GUIManager::Refresh()
{

	
}

void GUIManager::AddGUIObject(GUIObject* gobj, bool preserve )
{
	if (preserve)
		allGUIPreserved[gobj->name] = gobj;
	else
		allGUI[gobj->name] = gobj;

}

void GUIManager::Render(bool forceRefresh, bool forceClear)
{
	if (forceClear)
		Core::Instance().GetGraphicsAPI().ClearColorBuffer();

	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	
	SetBackgroundColor(1, 1, 1, 1);
	int x, y;
	Window::Instance().GetWindowSize(x, y);
	ImGui::SetNextWindowSize(ImVec2(x, y));

	ImGui::Begin("Hello, world!",0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowFontScale(1.5);

	for (auto it = allGUI.begin(); it != allGUI.end(); it++)
	{
		it->second->RenderImGuiElement();
	}

	for (auto it = allGUIPreserved.begin(); it != allGUIPreserved.end(); it++)
	{
		it->second->RenderImGuiElement();
	}


	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::PopStyleColor();
	if (forceRefresh)
		Window::Instance().Refresh();
}

void GUIManager::SelectFont(std::string fontName)
{
	auto it = allFonts.find(fontName);

	if (it != allFonts.end())
		ImGui::PushFont(allFonts[fontName]);
	else
		ImGui::PushFont(allFonts["defaultFont"]);
}


void GUIManager::DeleteGUIObjects(bool preservedToo)
{
	{

		auto it = allGUI.begin();

		for (; it != allGUI.end(); it++)
		{
			delete (it->second);
		}

		allGUI.clear();
	}

	{

	if (preservedToo)
	{
		auto it = allGUIPreserved.begin();

		for (; it != allGUIPreserved.end(); it++)
		{
			delete (it->second);
		}

		allGUIPreserved.clear();
	}
}


}


void GUIManager::SetBackgroundColor(float r, float g, float b, float a)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(r,g,b,a));
}
void GUIManager::Shutdown()
{
	// Cleanup
	DeleteGUIObjects(1);
	ImGui::GetIO().Fonts->ClearFonts();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
}

