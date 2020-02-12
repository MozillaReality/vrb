/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/AnimatedTransform.h"
#include "vrb/private/TransformState.h"
#include "vrb/private/UpdatableState.h"
#include "vrb/ConcreteClass.h"

#include "vrb/Matrix.h"
#include "vrb/RenderContext.h"

namespace vrb {

class Sampler {
public:
  virtual void Reset() {}
  virtual Matrix& Update(const float aDelta) = 0;
protected:
  Sampler() = default;
  ~Sampler() = default;
  Matrix mValue;
private:
  VRB_NO_DEFAULTS(Sampler)
};

typedef std::shared_ptr<Sampler> SamplerPtr;

class StaticTransformSampler: public Sampler {
public:
  static SamplerPtr Create(const Matrix& aTransform);
  StaticTransformSampler(const Matrix& aTransform);
  ~StaticTransformSampler() = default;
  Matrix& Update(const float aDelta) override;
protected:
private:
  StaticTransformSampler() = delete;
  VRB_NO_DEFAULTS(StaticTransformSampler)
  VRB_NO_NEW_DELETE
};

SamplerPtr
StaticTransformSampler::Create(const Matrix& aTransform) {
  return std::make_shared<StaticTransformSampler>(aTransform);
}

StaticTransformSampler::StaticTransformSampler(const Matrix& aTransform) {
  mValue = aTransform;
}

Matrix&
StaticTransformSampler::Update(const float) {
  return mValue;
}

class RotationSampler: public Sampler {
public:
  static SamplerPtr Create(const Vector& aAxis, const float aAngularVelocity);
  RotationSampler(const Vector& aAxis, const float aAngularVelocity);
  ~RotationSampler() = default;
  Matrix& Update(const float aDelta) override;
protected:
private:
  RotationSampler() = delete;
  VRB_NO_DEFAULTS(RotationSampler)
  VRB_NO_NEW_DELETE
  const Vector mAxis;
  const float mAngularVelocity;
};

SamplerPtr
RotationSampler::Create(const Vector& aAxis, const float aAngularVelocity) {
  return std::make_shared<RotationSampler>(aAxis, aAngularVelocity);
}

RotationSampler::RotationSampler(const Vector& aAxis, const float aAngularVelocity)
    : mAxis(aAxis), mAngularVelocity(aAngularVelocity) {}

Matrix&
RotationSampler::Update(const float aDelta) {
  mValue = Matrix::Rotation(mAxis, std::fmod((mAngularVelocity * aDelta), (2 * PI_FLOAT)));
  return mValue;
}

class TranslationSampler: public Sampler {
public:
  static SamplerPtr Create(const Vector& aDirection, const float aSpped);
  TranslationSampler(const Vector& aDirection, const float aSpped);
  ~TranslationSampler() = default;
  Matrix& Update(const float aDelta) override;
protected:
private:
  TranslationSampler() = delete;
  VRB_NO_DEFAULTS(TranslationSampler)
  VRB_NO_NEW_DELETE
  const Vector mDirection;
  const float mSpeed;
};

SamplerPtr
TranslationSampler::Create(const Vector& aDirection, const float aSpeed) {
  return std::make_shared<TranslationSampler>(aDirection, aSpeed);
}

TranslationSampler::TranslationSampler(const Vector& aDirection, const float aSpeed)
    : mDirection(aDirection.Normalize()), mSpeed(aSpeed < 0.0f ? -aSpeed : aSpeed) {}

Matrix&
TranslationSampler::Update(const float aDelta) {
  mValue = Matrix::Position(mDirection * (mSpeed * aDelta));
  return mValue;
}

struct AnimatedTransform::State : public Transform::State, Updatable::State {
  AnimationState state = AnimationState::Stop;
  Matrix startTransform;
  Matrix currentAnimationTransform;
  double startTime = -1.0;
  double lastAnimationTime = -1.0;
  float previousDeltaTime = 0.0;
  std::vector<SamplerPtr> samplers;
  void Reset() {
    state = AnimationState::Stop;
    currentAnimationTransform.SetIdentity();
    startTime = -1.0;
    lastAnimationTime = -1.0;
    previousDeltaTime = 0.0;
    for (SamplerPtr& sampler : samplers) {
      sampler->Reset();
    }
  }
};

AnimatedTransformPtr
AnimatedTransform::Create(CreationContextPtr& aContext) {
  AnimatedTransformPtr result = std::make_shared<ConcreteClass<AnimatedTransform, AnimatedTransform::State> >(aContext);
  result->m.self = result;
  return result;
}

void
AnimatedTransform::SetAnimationState(const AnimationState aState) {
  m.state = aState;

  if (aState == AnimationState::Stop) {
    if ((m.lastAnimationTime > 0.0) && (m.startTime > 0.0f) && (m.lastAnimationTime > m.startTime)) {
      m.previousDeltaTime += static_cast<float>(m.lastAnimationTime - m.startTime);
      m.startTime = -1.0f;
    }
  }
}

AnimatedTransform&
AnimatedTransform::ResetAnimations() {
  m.Reset();
  return *this;
}

AnimatedTransform&
AnimatedTransform::ClearAnimations() {
  m.Reset();
  m.samplers.clear();
  return *this;
}

AnimatedTransform&
AnimatedTransform::AddStaticTransform(const Matrix& aTransform) {
  m.samplers.emplace_back(StaticTransformSampler::Create(aTransform));
  return *this;
}


AnimatedTransform&
AnimatedTransform::AddRotationAnimation(const Vector& aAxis, const float aAngularVelocity) {
  m.samplers.emplace_back(RotationSampler::Create(aAxis, aAngularVelocity));
  return *this;
}

AnimatedTransform&
AnimatedTransform::AddTranslationAnimation(const Vector& aDirection, const float aSpeed) {
  m.samplers.emplace_back(TranslationSampler::Create(aDirection, aSpeed));
  return *this;
}

void
AnimatedTransform::SetTransform(const Matrix& aTransform) {
  m.startTransform = aTransform;
  Transform::SetTransform(aTransform.PreMultiply(m.currentAnimationTransform));
}

AnimatedTransform::AnimatedTransform(State& aState, CreationContextPtr& aContext) : m(aState), Transform(aState, aContext), Updatable(aState, aContext) {
  m.startTransform = m.transform;
  m.currentAnimationTransform.SetIdentity();
}

void
AnimatedTransform::UpdateResource(RenderContext& aContext) {
  if (m.state != AnimationState::Play) {
    return;
  }
  if (m.startTime < 0.0) {
    m.startTime = aContext.GetTimestamp();
  }

  m.lastAnimationTime = aContext.GetTimestamp();
  m.currentAnimationTransform.SetIdentity();
  const float delta = static_cast<float>(aContext.GetTimestamp() - m.startTime) + m.previousDeltaTime;
  for (SamplerPtr& sampler : m.samplers) {
    m.currentAnimationTransform.PreMultiplyInPlace(sampler->Update(delta));
  }
  m.transform = m.startTransform.PreMultiply(m.currentAnimationTransform);
}

} // namespace vrb

