/*
 * Copyright (c) 2014, Georgia Tech Research Corporation
 * All rights reserved.
 *
 * Author(s): Michael X. Grey <mxgrey@gatech.edu>
 *
 * Georgia Tech Graphics Lab and Humanoid Robotics Lab
 *
 * Directed by Prof. C. Karen Liu and Prof. Mike Stilman
 * <karenliu@cc.gatech.edu> <mstilman@cc.gatech.edu>
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DART_DYNAMICS_FRAME_H_
#define DART_DYNAMICS_FRAME_H_

#include <set>

#include <Eigen/Geometry>

#include "dart/dynamics/Entity.h"
#include "dart/math/MathTypes.h"

namespace dart {
namespace dynamics {

/// The Frame class serves as the backbone of DART's kinematic tree structure.
///
/// Frame inherits Entity, so it exists within a reference Frame. This class
/// keeps track of both its local (relative) and global (world) transforms,
/// velocities, and accelerations. It also notifies every child Entity when
class Frame : public virtual Entity
{
public:
  friend class Entity;
  friend class WorldFrame;

  /// Constructor for typical usage
  explicit Frame(const Frame* _refFrame, const std::string& _name);

  /// Destructor
  virtual ~Frame();

  static const Frame* World();

  //--------------------------------------------------------------------------
  // Transform
  //--------------------------------------------------------------------------

  /// Get the transform of this Frame with respect to its parent Frame
  virtual const Eigen::Isometry3d& getRelativeTransform() const = 0;

  /// Get the transform of this Frame with respect to the World Frame
  const Eigen::Isometry3d& getWorldTransform() const;

  /// Get the transform of this Frame with respect to some other Frame
  Eigen::Isometry3d getTransform(
      const Frame* _withRespectTo = Frame::World()) const;

  //-------------------------------------------------------------------------
  // Velocity
  //-------------------------------------------------------------------------

  /// Get the spatial velocity of this Frame relative to its parent Frame, in
  /// its own coordinates.
  virtual const Eigen::Vector6d& getRelativeSpatialVelocity() const = 0;

  /// Get the total spatial velocity of this Frame in the coordinates of this
  /// Frame.
  const Eigen::Vector6d& getSpatialVelocity() const;

  /// Get the total spatial velocity of this Frame. The velocity can be provided
  /// in the coordinates of any Frame.
  Eigen::Vector6d getSpatialVelocity(const Frame* _inCoordinates) const;

  /// Get the linear portion of classical velocity of this Frame relative to
  /// some other Frame. It can be expressed in the coordinates of any Frame.
  Eigen::Vector3d getLinearVelocity(
      const Frame* _relativeTo = Frame::World(),
      const Frame* _inCoordinates = Frame::World()) const;

  /// Get the angular portion of classical velocity of this Frame relative to
  /// some other Frame. It can be expressed in the coordinates of any Frame.
  Eigen::Vector3d getAngularVelocity(
      const Frame* _relativeTo = Frame::World(),
      const Frame* _inCoordinates = Frame::World()) const;

  //--------------------------------------------------------------------------
  // Acceleration
  //--------------------------------------------------------------------------

  /// Get the spatial acceleration of this Frame relative to its parent Frame,
  /// in the coordinates of this Frame.
  virtual const Eigen::Vector6d& getRelativeSpatialAcceleration() const = 0;

  /// Get the total spatial acceleration of this Frame in the coordinates of
  /// this Frame.
  const Eigen::Vector6d& getSpatialAcceleration() const;

  /// Get the total spatial acceleration of this Frame. The acceleration can be
  /// provided in the coordinates of any Frame.
  Eigen::Vector6d getSpatialAcceleration(const Frame* _inCoordinates) const;

  /// Get the linear portion of classical acceleration of this Frame relative to
  /// some other Frame. It can be expressed in the coordinates of any Frame.
  Eigen::Vector3d getLinearAcceleration(
      const Frame* _relativeTo=Frame::World(),
      const Frame* _inCoordinates=Frame::World()) const;

  Eigen::Vector3d getAngularAcceleration(
      const Frame* _relativeTo=Frame::World(),
      const Frame* _inCoordinates=Frame::World()) const;

  //--------------------------------------------------------------------------
  // Relationships
  //--------------------------------------------------------------------------

  /// Get a container with the Entities that are children of this Frame.
  /// std::set is used because Entities may be arbitrarily added and removed
  /// from a parent Frame, and each entry should be unique.
  const std::set<Entity*>& getChildEntities();

  /// Get a container with the Entities that are children of this Frame. Note
  /// that this is version is less efficient than the non-const version because
  /// it needs to rebuild a set so that the entries are const.
  std::set<const Entity*> getChildEntities() const;

  /// Get the number of Entities that are currently children of this Frame.
  size_t getNumChildEntities() const;

  /// Get a container with the Frames that are children of this Frame.
  /// std::set is used because Frames may be arbitrarily added and removed
  /// from a parent Frame, and each entry should be unique.
  const std::set<Frame*>& getChildFrames();

  /// Get a container with the Frames that are children of this Frame. Note
  /// that this version is less efficient than the non-const version because
  /// it needs to rebuild a set so that the entries are const.
  std::set<const Frame*> getChildFrames() const;

  /// Get the number of Frames that are currently children of this Frame.
  size_t getNumChildFrames() const;

  /// Returns true if this Frame is the World Frame
  bool isWorld() const;

  //--------------------------------------------------------------------------
  // Rendering
  //--------------------------------------------------------------------------

  // Render this Frame as well as any Entities it contains
  virtual void draw(renderer::RenderInterface *_ri = NULL,
                    const Eigen::Vector4d &_color = Eigen::Vector4d::Ones(),
                    bool _useDefaultColor = true, int _depth = 0) const;

  /// Notify this Frame and all its children that its pose has changed
  virtual void notifyTransformUpdate();

  /// Notify this Frame and all its children that its velocity has changed
  virtual void notifyVelocityUpdate();

  /// Notify this Frame and all its children that its acceleration has changed
  virtual void notifyAccelerationUpdate();

protected:
  // Documentation inherited
  virtual void changeParentFrame(const Frame* _newParentFrame);

private:
  /// Constructor for World Frame
  explicit Frame();

protected:
  /// World transform of this Frame. This object is mutable to enable
  /// auto-updating to happen in the const member getWorldTransform() function
  mutable Eigen::Isometry3d mWorldTransform;

  /// Total velocity of this Frame, in the coordinates of this Frame
  mutable Eigen::Vector6d mVelocity;

  /// Total acceleration of this Frame, in the coordinates of this Frame
  mutable Eigen::Vector6d mAcceleration;

  /// Container of this Frame's child Frames. This object is mutable to enable
  /// children to be added to const Frames. Receiving a new child does not
  /// affect any properties of the parent Frame.
  mutable std::set<Frame*> mChildFrames;

  /// Container of this Frame's child Entities. This object is mutable to enable
  /// children to be added to const Frames. Receiving a new child does not
  /// affect any properties of the parent Frame.
  mutable std::set<Entity*> mChildEntities;

private:
  /// Contains whether or not this is the World Frame
  bool mAmWorld;

};

class WorldFrame : public Frame
{
public:
  friend class Frame;

  /// Always returns the Identity Transform
  const Eigen::Isometry3d& getRelativeTransform() const;

  /// Always returns a zero vector
  const Eigen::Vector6d& getRelativeSpatialVelocity() const;

  /// Always returns a zero vector
  const Eigen::Vector6d& getRelativeSpatialAcceleration() const;

private:
  /// This may only be constructed by the Frame class
  explicit WorldFrame();

private:
  /// This is set to Identity and never changes
  Eigen::Isometry3d mRelativeTf;

  /// This is set to a Zero vector and never changes
  Eigen::Vector6d mRelativeVelocity;

  /// This is set to a Zero vector and never changes
  Eigen::Vector6d mRelativeAcceleration;

};

} // namespace dynamics
} // namespace dart

#endif // DART_DYNAMICS_FRAME_H_