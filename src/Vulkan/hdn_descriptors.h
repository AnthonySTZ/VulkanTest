#pragma once

#include "hdn_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace hdn {

    
class HdnDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(HdnDevice &device) : hdnDevice{device} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<HdnDescriptorSetLayout> build() const;

   private:
    HdnDevice &hdnDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  HdnDescriptorSetLayout(
      HdnDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~HdnDescriptorSetLayout();
  HdnDescriptorSetLayout(const HdnDescriptorSetLayout &) = delete;
  HdnDescriptorSetLayout &operator=(const HdnDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  HdnDevice &hdnDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class HdnDescriptorWriter;
};



class HdnDescriptorPool {
 public:
  class Builder {
   public:
    Builder(HdnDevice &device) : hdnDevice{device} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<HdnDescriptorPool> build() const;

   private:
    HdnDevice &hdnDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  HdnDescriptorPool(
      HdnDevice &device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~HdnDescriptorPool();
  HdnDescriptorPool(const HdnDescriptorPool &) = delete;
  HdnDescriptorPool &operator=(const HdnDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  VkDescriptorPool getPool() const { return descriptorPool; };

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  HdnDevice &hdnDevice;
  VkDescriptorPool descriptorPool;

  friend class HdnDescriptorWriter;
};



class HdnDescriptorWriter {
 public:
  HdnDescriptorWriter(HdnDescriptorSetLayout &setLayout, HdnDescriptorPool &pool);

  HdnDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  HdnDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  HdnDescriptorSetLayout &setLayout;
  HdnDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}