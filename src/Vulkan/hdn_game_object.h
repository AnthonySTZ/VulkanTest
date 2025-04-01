#pragma once

#include "hdn_model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace hdn {
	struct TransformComponent {
		glm::vec3 translation;
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// T * Ry * Rx * Rz * Scale
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float LightIntensity = 1.0f;
	};

	class HdnGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, HdnGameObject>;

		static HdnGameObject createGameObject() {
			static id_t currentId = 0;
			return HdnGameObject(currentId++);
		};

		static HdnGameObject makePointLight(float intensity=1.f, float radius = 0.05f, glm::vec3 color = glm::vec3(1.f) );

		HdnGameObject(const HdnGameObject&) = delete;
		HdnGameObject& operator=(const HdnGameObject&) = delete;
		HdnGameObject(HdnGameObject&&) = default;
		HdnGameObject& operator=(HdnGameObject&&) = default;

		id_t getId() const { return id; };

		glm::vec3 color{};
		TransformComponent transform{};
		
		// optional pointer components 
		std::shared_ptr<HdnModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		HdnGameObject(id_t objId) : id{ objId } {};
		id_t id;
	};
}