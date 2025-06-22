
#include "filesystem"

#include "game/game_manager.h"

#include "utils/log.h"

#include "maths/basic.h"
#include "utils/conversion.h"

#include <fmt/format.h>
#include <imgui.h>
#include <chrono>

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#endif

namespace game
{

GameManager::GameManager() :
	transformManager_(entityManager_),
	rollbackManager_(*this, entityManager_)
{
	playerEntityMap_.fill(core::INVALID_ENTITY);
}

void GameManager::SpawnPlayer(PlayerNumber playerNumber, core::Vec2f position, core::Vec2f direction)
{
	if (GetEntityFromPlayerNumber(playerNumber) != core::INVALID_ENTITY)
		return;
	core::LogDebug("[GameManager] Spawning new player");
	const auto entity = entityManager_.CreateEntity();
	playerEntityMap_[playerNumber] = entity;

	transformManager_.AddComponent(entity);
	transformManager_.SetPosition(entity, position);
	rollbackManager_.SpawnPlayer(playerNumber, entity, position, direction);
}
core::Entity GameManager::GetEntityFromPlayerNumber(PlayerNumber playerNumber) const
{
	return playerEntityMap_[playerNumber];
}
void GameManager::SetPlayerInput(PlayerNumber playerNumber, PlayerInput playerInput, std::uint32_t inputFrame)
{
	if (playerNumber == INVALID_PLAYER)
		return;

	rollbackManager_.SetPlayerInput(playerNumber, playerInput, inputFrame);

}
void GameManager::Validate(Frame newValidateFrame)
{

#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if (rollbackManager_.GetCurrentFrame() < newValidateFrame)
	{
		rollbackManager_.StartNewFrame(newValidateFrame);
	}
	rollbackManager_.ValidateFrame(newValidateFrame);
}
core::Entity GameManager::SpawnBullet(PlayerNumber playerNumber, core::Vec2f position, core::Vec2f velocity)
{
	const core::Entity entity = entityManager_.CreateEntity();

	transformManager_.AddComponent(entity);
	transformManager_.SetPosition(entity, position);
	transformManager_.SetScale(entity, core::Vec2f::one() * BULLET_SCALE);
	transformManager_.SetRotation(entity, core::Degree(0.0f));
	rollbackManager_.SpawnBullet(playerNumber, entity, position, velocity);
	return entity;
}

void GameManager::DestroyBullet(core::Entity entity)
{
	rollbackManager_.DestroyEntity(entity);
}
PlayerNumber GameManager::CheckWinner() const
{
	int alivePlayer = 0;
	PlayerNumber winner = INVALID_PLAYER;
	const auto& playerManager = rollbackManager_.GetPlayerCharacterManager();
	for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
	{
		if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER)))
			continue;
		const auto& player = playerManager.GetComponent(entity);
		if (player.health > 0)
		{
			alivePlayer++;
			winner = player.playerNumber;
		}
	}

	return alivePlayer == 1 ? winner : INVALID_PLAYER;
}
void GameManager::WinGame(PlayerNumber winner)
{
	winner_ = winner;
}

ClientGameManager::ClientGameManager(PacketSenderInterface& packetSenderInterface) :
	GameManager(),
	packetSenderInterface_(packetSenderInterface),
	spriteManager_(entityManager_, transformManager_),
	animationManager_(entityManager_, spriteManager_, *this),
	soundManager_(entityManager_, *this)
{
	healthBarMap.fill(core::INVALID_ENTITY);
}

void ClientGameManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	//load fonts
	if (!font_.loadFromFile("data/fonts/8-bit-hud.ttf"))
	{
		core::LogError("Could not load font");
	}
	textRenderer_.setFont(font_);
	
	//load Environment elements
	LoadBackground("background");
	if(!wallTexture_.loadFromFile("data/sprites/wall.png"))
	{
		core::LogError("Could not wall sprite");
	}
	CreateBackground();

	//load PlayerCharacter Textures
	animationManager_.LoadTexture("cat_idle", animationManager_.catIdle);
	animationManager_.LoadTexture("cat_walk", animationManager_.catWalk);
	animationManager_.LoadTexture("cat_jump", animationManager_.catJump);
	animationManager_.LoadTexture("cat_shoot", animationManager_.catShoot);

	if (!bulletTexture_.loadFromFile("data/sprites/bullet.png"))
	{
		core::LogError("Could not load bullet sprite");
	}

	//load sounds
	soundManager_.LoadSound("cat_hiss", soundManager_.catHissSound);
	soundManager_.LoadSound("cat_jump", soundManager_.catJumpSound);

}
void ClientGameManager::Update(sf::Time dt)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if (state_ & STARTED)
	{
		rollbackManager_.SimulateToCurrentFrame();
		//Copy rollback transform position to our own
		for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
		{
			//Update Entities (BULLET)
			if (entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::BULLET)))
			{
				transformManager_.SetScale(entity, rollbackManager_.GetTransformManager().GetScale(entity));
				transformManager_.SetPosition(entity, rollbackManager_.GetTransformManager().GetPosition(entity));
				transformManager_.SetRotation(entity, rollbackManager_.GetTransformManager().GetRotation(entity));
			}
			//Update Entities with PLAYER_CHARACTER
			if (entityManager_.HasComponent(entity,
				static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER) |
				static_cast<core::EntityMask>(core::ComponentType::SPRITE) |
				static_cast<core::EntityMask>(core::ComponentType::TRANSFORM)))
			{
				auto& player = rollbackManager_.GetPlayerCharacterManager().GetComponent(entity);

				if (player.invincibilityTime > 0.0f)
				{
					auto leftV = std::fmod(player.invincibilityTime, INVINCIBILITY_FLASH_PERIOD);
					auto rightV = INVINCIBILITY_FLASH_PERIOD / 2.0f;
					core::LogDebug(fmt::format("Comparing {} and {} with time: {}", leftV, rightV, player.invincibilityTime));
				}

				if (player.invincibilityTime > 0.0f &&
					std::fmod(player.invincibilityTime, INVINCIBILITY_FLASH_PERIOD) > INVINCIBILITY_FLASH_PERIOD / 2.0f)
				{
					spriteManager_.SetColor(entity, sf::Color::Black);
				}
				else
				{
					spriteManager_.SetColor(entity, PLAYER_COLORS[player.playerNumber]);
				}
				//Updates the animations
				animationManager_.UpdateEntity(entity, player.animationState, dt);
				//Plays the correct sound on the entity according to its state
				soundManager_.PlaySound(entity);

				transformManager_.SetPosition(entity, rollbackManager_.GetTransformManager().GetPosition(entity));
				transformManager_.SetScale(entity, core::Vec2f{ player.lookDir.x * PLAYER_SCALE.x, PLAYER_SCALE.y });
				transformManager_.SetRotation(entity, rollbackManager_.GetTransformManager().GetRotation(entity));
			}
		}
	}
	fixedTimer_ += dt.asSeconds();
	while (fixedTimer_ > FIXED_PERIOD)
	{
		FixedUpdate();
		fixedTimer_ -= FIXED_PERIOD;
	}
}
void ClientGameManager::End()
{
}
void ClientGameManager::SetWindowSize(sf::Vector2u windowsSize)
{
	windowSize_ = windowsSize;
	const sf::FloatRect visibleArea(0.0f, 0.0f,
		static_cast<float>(windowSize_.x),
		static_cast<float>(windowSize_.y));
	originalView_ = sf::View(visibleArea);
	spriteManager_.SetWindowSize(sf::Vector2f(windowsSize));
	spriteManager_.SetCenter(sf::Vector2f(windowsSize) / 2.0f);
	auto& currentPhysicsManager = rollbackManager_.GetCurrentPhysicsManager();
	currentPhysicsManager.SetCenter(sf::Vector2f(windowsSize) / 2.0f);
	currentPhysicsManager.SetWindowSize(sf::Vector2f(windowsSize));
}
void ClientGameManager::Draw(sf::RenderTarget& target)
{

#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	UpdateCameraView();
	target.setView(cameraView_);

	spriteManager_.Draw(target);

	if (drawPhysics_)
	{
		auto& currentPhysicsManager = rollbackManager_.GetCurrentPhysicsManager();
		currentPhysicsManager.Draw(target);
	}

	// Draw texts on screen
	target.setView(originalView_);
	if (state_ & FINISHED)
	{
		if (winner_ == GetPlayerNumber())
		{
			const std::string winnerText = fmt::format("You won!");
			textRenderer_.setFillColor(sf::Color::White);
			textRenderer_.setString(winnerText);
			textRenderer_.setCharacterSize(32);
			const auto textBounds = textRenderer_.getLocalBounds();
			textRenderer_.setPosition(static_cast<float>(windowSize_.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(windowSize_.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(textRenderer_);
		}
		else if (winner_ != INVALID_PLAYER)
		{
			const std::string winnerText = fmt::format("P{} won!", winner_ + 1);
			textRenderer_.setFillColor(sf::Color::White);
			textRenderer_.setString(winnerText);
			textRenderer_.setCharacterSize(32);
			const auto textBounds = textRenderer_.getLocalBounds();
			textRenderer_.setPosition(static_cast<float>(windowSize_.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(windowSize_.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(textRenderer_);
		}
		else
		{
			const std::string errorMessage = fmt::format("Error with other players");
			textRenderer_.setFillColor(sf::Color::Red);
			textRenderer_.setString(errorMessage);
			textRenderer_.setCharacterSize(32);
			const auto textBounds = textRenderer_.getLocalBounds();
			textRenderer_.setPosition(static_cast<float>(windowSize_.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(windowSize_.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(textRenderer_);
		}
	}
	if (!(state_ & STARTED))
	{
		if (startingTime_ != 0)
		{
			using namespace std::chrono;
			unsigned long long ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				).count();
			if (ms < startingTime_)
			{
				const std::string countDownText = fmt::format("Starts in {}", ((startingTime_ - ms) / 1000 + 1));
				textRenderer_.setFillColor(sf::Color::White);
				textRenderer_.setString(countDownText);
				textRenderer_.setCharacterSize(32);
				const auto textBounds = textRenderer_.getLocalBounds();
				textRenderer_.setPosition(static_cast<float>(windowSize_.x) / 2.0f - textBounds.width / 2.0f,
					static_cast<float>(windowSize_.y) / 2.0f - textBounds.height / 2.0f);
				target.draw(textRenderer_);
			}
		}
	}
	else
	{
		const auto& playerManager = rollbackManager_.GetPlayerCharacterManager();
		for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
		{
			const auto playerEntity = GetEntityFromPlayerNumber(playerNumber);
			if (playerEntity == core::INVALID_ENTITY)
			{
				continue;
			}
			transformManager_.SetScale(healthBarMap[playerNumber], core::Vec2f{
			HEALTH_BAR_SCALE.x * static_cast<float>(playerManager.GetComponent(playerEntity).health)/ static_cast<float>(PLAYER_HEALTH),
				HEALTH_BAR_SCALE.y });
		}
	}
}
void ClientGameManager::SetClientPlayer(PlayerNumber clientPlayer)
{
	clientPlayer_ = clientPlayer;
}
void ClientGameManager::SpawnPlayer(PlayerNumber playerNumber, core::Vec2f position, core::Vec2f direction)
{
	core::LogDebug(fmt::format("Spawn player: {}", playerNumber));

	GameManager::SpawnPlayer(playerNumber, position, direction);
	const auto entity = GetEntityFromPlayerNumber(playerNumber);
	spriteManager_.AddComponent(entity);
	spriteManager_.SetTexture(entity, animationManager_.catIdle.textures[0]);
	spriteManager_.SetOrigin(entity, sf::Vector2f(animationManager_.catIdle.textures[0].getSize()) / 2.0f);
	spriteManager_.SetColor(entity, PLAYER_COLORS[playerNumber]);
	animationManager_.AddComponent(entity);
	soundManager_.AddComponent(entity);
}
core::Entity ClientGameManager::SpawnBullet(PlayerNumber playerNumber, core::Vec2f position, core::Vec2f velocity)
{
	const auto entity = GameManager::SpawnBullet(playerNumber, position, velocity);

	spriteManager_.AddComponent(entity);
	spriteManager_.SetTexture(entity, bulletTexture_);
	spriteManager_.SetOrigin(entity, sf::Vector2f(bulletTexture_.getSize()) / 2.0f);
	spriteManager_.SetColor(entity, PLAYER_COLORS[playerNumber]);

	return entity;
}
void ClientGameManager::FixedUpdate()
{

#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if (!(state_ & STARTED))
	{
		if (startingTime_ != 0)
		{
			using namespace std::chrono;
			const auto ms = duration_cast<duration<unsigned long long, std::milli>>(
				system_clock::now().time_since_epoch()
				).count();
			if (ms > startingTime_)
			{
				state_ = state_ | STARTED;
			}
			else
			{

				return;
			}
		}
		else
		{
			return;
		}
	}
	if (state_ & FINISHED)
	{
		return;
	}

	//We send the player inputs when the game started
	const auto playerNumber = GetPlayerNumber();
	if (playerNumber == INVALID_PLAYER)
	{
		//We still did not receive the spawn player packet, but receive the start game packet
		core::LogWarning(fmt::format("Invalid Player Entity in {}:line {}", __FILE__, __LINE__));
		return;
	}
	const auto& inputs = rollbackManager_.GetInputs(playerNumber);
	auto playerInputPacket = std::make_unique<PlayerInputPacket>();
	playerInputPacket->playerNumber = playerNumber;
	playerInputPacket->currentFrame = core::ConvertToBinary(currentFrame_);
	for (size_t i = 0; i < playerInputPacket->inputs.size(); i++)
	{
		if (i > currentFrame_)
		{
			break;
		}

		playerInputPacket->inputs[i] = inputs[i];
	}
	packetSenderInterface_.SendUnreliablePacket(std::move(playerInputPacket));


	currentFrame_++;
	rollbackManager_.StartNewFrame(currentFrame_);
}
void ClientGameManager::SetPlayerInput(PlayerNumber playerNumber, PlayerInput playerInput, std::uint32_t inputFrame)
{
	if (playerNumber == INVALID_PLAYER)
		return;
	GameManager::SetPlayerInput(playerNumber, playerInput, inputFrame);
}
void ClientGameManager::StartGame(unsigned long long int startingTime)
{
	core::LogDebug(fmt::format("Start game at starting time: {}", startingTime));
	startingTime_ = startingTime;
}
void ClientGameManager::DrawImGui()
{
	ImGui::SetWindowFontScale(2.0f);
	ImGui::Text(state_ & STARTED ? "Game has started" : "Game has not started");
	if (startingTime_ != 0)
	{
		ImGui::Text("Starting Time: %llu", startingTime_);
		using namespace std::chrono;
		unsigned long long ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
			).count();
		ImGui::Text("Current Time: %llu", ms);
	}
	ImGui::Checkbox("Draw Physics", &drawPhysics_);
}
void ClientGameManager::ConfirmValidateFrame(Frame newValidateFrame,
	const std::array<PhysicsState, MAX_PLAYER_NMB>& physicsStates)
{
	if (newValidateFrame < rollbackManager_.GetLastValidateFrame())
	{
		core::LogWarning(fmt::format("New validate frame is too old"));
		return;
	}
	for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
	{
		if (rollbackManager_.GetLastReceivedFrame(playerNumber) < newValidateFrame)
		{

			core::LogWarning(fmt::format("Trying to validate frame {} while playerNumber {} is at input frame {}, client player {}",
				newValidateFrame,
				playerNumber + 1,
				rollbackManager_.GetLastReceivedFrame(playerNumber),
				GetPlayerNumber() + 1));


			return;
		}
	}
	rollbackManager_.ConfirmFrame(newValidateFrame, physicsStates);
}
void ClientGameManager::WinGame(PlayerNumber winner)
{
	GameManager::WinGame(winner);
	state_ = state_ | FINISHED;
}
void ClientGameManager::UpdateCameraView()
{
	if ((state_ & STARTED) != STARTED)
	{
		cameraView_ = originalView_;
		return;
	}

	cameraView_ = originalView_;
	const sf::Vector2f extends{ cameraView_.getSize() / 2.0f / core::PIXEL_PER_METER };
	float currentZoom = 1.0f;
	constexpr float margin = 1.0f;
	for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
	{
		const auto playerEntity = GetEntityFromPlayerNumber(playerNumber);
		if (playerEntity == core::INVALID_ENTITY)
		{
			continue;
		}
		if (entityManager_.HasComponent(playerEntity, static_cast<core::EntityMask>(core::ComponentType::POSITION)))
		{
			const auto position = transformManager_.GetPosition(playerEntity);
			if (core::Abs(position.x) + margin > extends.x)
			{
				const auto ratio = (std::abs(position.x) + margin) / extends.x;
				if (ratio > currentZoom)
				{
					currentZoom = ratio;
				}
			}
			if (core::Abs(position.y) + margin > extends.y)
			{
				const auto ratio = (std::abs(position.y) + margin) / extends.y;
				if (ratio > currentZoom)
				{
					currentZoom = ratio;
				}
			}
		}
	}
	cameraView_.zoom(currentZoom);

}
void ClientGameManager::LoadBackground(const std::string_view path)
{
	auto format = fmt::format("data/sprites/{}", path);
	auto dirIter = std::filesystem::directory_iterator(fmt::format("data/sprites/{}", path));
	const int textureCount = std::count_if(
		begin(dirIter),
		end(dirIter),
		[](auto& entry) { return entry.is_regular_file() && entry.path().extension() == ".png"; });

	//LOAD SPRITES
	for (size_t i = 0; i < textureCount; i++)
	{
		sf::Texture newTexture;
		const auto fullPath = fmt::format("data/sprites/{}/{}{}.png", path, path, i);
		if (!newTexture.loadFromFile(fullPath))
		{
			core::LogError(fmt::format("Could not load data/sprites/{}/{}{}.png sprite", path, path, i));
		}
		backgroundTextures_.push_back(newTexture);
	}
}
void ClientGameManager::CreateBackground()
{
	for (auto& background : backgroundTextures_)
	{
		const core::Entity entity = entityManager_.CreateEntity();
		spriteManager_.AddComponent(entity);
		spriteManager_.SetTexture(entity, background);
		transformManager_.AddComponent(entity);
		transformManager_.SetScale(entity, core::Vec2f(
			(static_cast<float>(windowSize_.x) / core::PIXEL_PER_METER ) * 1.5f,
			(static_cast<float>(windowSize_.y)/core::PIXEL_PER_METER) * 1.5f));
		spriteManager_.SetOrigin(entity, sf::Vector2f(background.getSize()) / 2.0f);
	}

	const core::Entity entity = entityManager_.CreateEntity();
	spriteManager_.AddComponent(entity);
	spriteManager_.SetTexture(entity, wallTexture_);
	transformManager_.AddComponent(entity);
	transformManager_.SetScale(entity, core::Vec2f(
		(static_cast<float>(windowSize_.x)/core::PIXEL_PER_METER) * 10.0f,
		(static_cast<float>(windowSize_.y) / core::PIXEL_PER_METER)* 0.8f));
	transformManager_.SetPosition(entity, core::Vec2f(0.0f, -9.0f));
	spriteManager_.SetOrigin(entity, sf::Vector2f(wallTexture_.getSize()) / 2.0f);
	spriteManager_.SetColor(entity, FLOOR_COLOR);
}

void ClientGameManager::CreateHealthBar(PlayerNumber playerNumber)
{
	if (healthBarMap[playerNumber] != core::INVALID_ENTITY)
		return;
	const core::Entity backHealthBar = entityManager_.CreateEntity();
	const core::Entity entity = entityManager_.CreateEntity();
	healthBarMap[playerNumber] = entity;

	spriteManager_.AddComponent(backHealthBar);
	spriteManager_.SetTexture(backHealthBar, wallTexture_);
	spriteManager_.SetOrigin(backHealthBar, sf::Vector2f(0.0f, static_cast<float>(wallTexture_.getSize().y) / 2.0f));
	core::Color backHealthColorBar = PLAYER_COLORS[playerNumber];
	backHealthColorBar.a = 110u;
	spriteManager_.SetColor(backHealthBar, backHealthColorBar);

	transformManager_.AddComponent(backHealthBar);
	transformManager_.SetScale(backHealthBar, core::Vec2f{ HEALTH_BAR_SCALE.x, HEALTH_BAR_SCALE.y });
	transformManager_.SetPosition(backHealthBar,
		core::Vec2f{ HEALTHBAR_POSITION[playerNumber],HEALTH_BAR_HEIGHT });

	spriteManager_.AddComponent(entity);
	spriteManager_.SetTexture(entity, wallTexture_);
	spriteManager_.SetOrigin(entity, sf::Vector2f(0.0f, static_cast<float>(wallTexture_.getSize().y) / 2.0f));
	spriteManager_.SetColor(entity, PLAYER_COLORS[playerNumber]);

	transformManager_.AddComponent(entity);
	transformManager_.SetScale(entity, core::Vec2f{ HEALTH_BAR_SCALE.x, HEALTH_BAR_SCALE.y });
	transformManager_.SetPosition(entity, 
		core::Vec2f{ HEALTHBAR_POSITION[playerNumber],HEALTH_BAR_HEIGHT });

	return;
}
}
