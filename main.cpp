#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

// シーン管理をカプセル化したクラス
class SceneManager {
public:
	// シーンの種類（外部から初期シーンを指定できるようpublicに配置）
	enum class Scene {
		kUnknown = 0,
		kTitle,
		kGame,
	};

	// デストラクタ（インスタンス破棄時に自動でメモリ解放する）
	~SceneManager() {
		delete gameScene_;
		delete titleScene_;
	}

	// 初期化（最初のシーンを設定）
	void Initialize(Scene initialScene) {
		scene_ = initialScene;
		if (scene_ == Scene::kTitle) {
			titleScene_ = new TitleScene();
			titleScene_->Initialize();
		} else if (scene_ == Scene::kGame) {
			gameScene_ = new GameScene();
			gameScene_->Initialize();
		}
	}

	// 更新処理（シーン切り替え ＋ 現在のシーンの更新）
	void Update() {
		ChangeScene();

		switch (scene_) {
		case Scene::kTitle:
			titleScene_->Update();
			break;
		case Scene::kGame:
			gameScene_->Update();
			break;
		}
	}

	// 描画処理
	void Draw() {
		switch (scene_) {
		case Scene::kTitle:
			titleScene_->Draw();
			break;
		case Scene::kGame:
			gameScene_->Draw();
			break;
		}
	}

private:
	// メンバ変数（外部からの直接書き換えを禁止）
	GameScene* gameScene_ = nullptr;
	TitleScene* titleScene_ = nullptr;
	Scene scene_ = Scene::kUnknown;

	// シーン切り替え処理（Updateの内部で自動的に呼ばれるためprivate）
	void ChangeScene() {
		switch (scene_) {
		case Scene::kTitle:
			if (titleScene_->IsFinished()) {
				scene_ = Scene::kGame;

				delete titleScene_;
				titleScene_ = nullptr;

				gameScene_ = new GameScene();
				gameScene_->Initialize();
			}
			break;
		case Scene::kGame:
			// ※元のコードのスペルに合わせて IsFnished() にしています
			if (gameScene_->IsFnished()) {
				scene_ = Scene::kTitle;

				delete gameScene_;
				gameScene_ = nullptr;

				titleScene_ = new TitleScene();
				titleScene_->Initialize();
			}
			break;
		}
	}
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーンマネージャーの生成と初期化
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Initialize(SceneManager::Scene::kGame);

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		// シーンの更新（切り替え判定も含む）
		sceneManager->Update();

		dxCommon->PreDraw();

		// シーンの描画
		sceneManager->Draw();

		dxCommon->PostDraw();
	}

	// 解放処理（デストラクタが呼ばれ、中のSceneも自動でdeleteされる）
	delete sceneManager;

	KamataEngine::Finalize();

	return 0;
}