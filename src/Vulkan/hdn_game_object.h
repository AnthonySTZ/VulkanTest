#pragma once

#include "hdn_model.h"

#include <memory>

namespace hdn {

	struct Transform2dComponent {
		glm::vec2 translation;
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() const {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			glm::mat2 rotMat{ {c, s}, {-s, c} }; // Glm uses column instead of rows
			glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
			return rotMat * scaleMat;
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
		Transform2dComponent transform2d{};

	private:
		HdnGameObject(id_t objId) : id{ objId } {};
		id_t id;
	};
}