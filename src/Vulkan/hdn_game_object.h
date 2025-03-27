#pragma once

#include "hdn_model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace hdn {

	struct TransformComponent {
		glm::vec3 translation;
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// T * Ry * Rx * Rz * Scale
		glm::mat4 mat4() const {			
			auto transform = glm::translate(glm::mat4{1.f}, translation);

			transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
			transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
			transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});

			transform = glm::scale(transform, scale);
			return transform;
		};
	};

	class HdnGameObject {
	public:
		using id_t = unsigned int;

		static HdnGameObject createGameObject() {
			static id_t currentId = 0;
			return HdnGameObject(currentId++);
		};

		HdnGameObject(const HdnGameObject&) = delete;
		HdnGameObject& operator=(const HdnGameObject&) = delete;
		HdnGameObject(HdnGameObject&&) = default;
		HdnGameObject& operator=(HdnGameObject&&) = default;

		id_t getId() const { return id; };

		std::shared_ptr<HdnModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		HdnGameObject(id_t objId) : id{ objId } {};
		id_t id;
	};
}