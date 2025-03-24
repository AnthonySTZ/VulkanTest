#pragma once


namespace hdn {

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

	private:
		HdnGameObject(id_t objId) : id{ objId } {};
		id_t id;
	};
}