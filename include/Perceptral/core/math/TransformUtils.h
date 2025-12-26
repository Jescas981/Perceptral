#pragma once

#include <Eigen/Eigen>
#include <Perceptral/scene/Components.h>


namespace Perceptral::Math {

inline Eigen::Vector3f right(const Component::Transform &t) {
  return t.rotation * Eigen::Vector3f::UnitX();
}

inline Eigen::Vector3f forward(const Component::Transform &t) {
  return t.rotation * Eigen::Vector3f::UnitY();
}

inline Eigen::Vector3f up(const Component::Transform &t) {
  return t.rotation * Eigen::Vector3f::UnitZ();
}

inline Eigen::Vector3f left(const Component::Transform &t) { return -right(t); }

inline Eigen::Vector3f backward(const Component::Transform &t) {
  return -forward(t);
}

inline Eigen::Vector3f down(const Component::Transform &t) { return -up(t); }

/// Build world matrix from Transform (no scale)
inline Eigen::Matrix4f toMatrix(const Component::Transform &t) {
  Eigen::Matrix4f m = Eigen::Matrix4f::Identity();

  m.block<3, 3>(0, 0) = t.rotation.toRotationMatrix();
  m.block<3, 1>(0, 3) = t.translation;

  return m;
}

inline Eigen::Matrix3f lookRotation(const Eigen::Vector3f &forward,
                                    const Eigen::Vector3f &up) {
  Eigen::Vector3f f = forward.normalized();
  Eigen::Vector3f r = f.cross(up).normalized();
  Eigen::Vector3f u = r.cross(f);
  Eigen::Matrix3f m;
  m.col(0) = r;
  m.col(1) = f;
  m.col(2) = u;
  return m;
}

inline Eigen::Matrix4f
lookAt(const Eigen::Vector3f &position, const Eigen::Vector3f &target,
       const Eigen::Vector3f &worldUp = Eigen::Vector3f::UnitZ()) {
  // Forward vector: from position to target (camera looks at target)
  Eigen::Vector3f front = (target - position).normalized();

  // Right vector: perpendicular to forward and world up
  Eigen::Vector3f right = front.cross(worldUp).normalized();

  // Real up vector: perpendicular to forward and right
  Eigen::Vector3f up = right.cross(front).normalized();

  Eigen::Matrix4f view_matrix;
  view_matrix << right.x(), right.y(), right.z(), -right.dot(position), //
      up.x(), up.y(), up.z(), -up.dot(position),                        //
      -front.x(), -front.y(), -front.z(), front.dot(position),          //
      0.0f, 0.0f, 0.0f, 1.0f;

  return view_matrix;
}

/// Build view matrix (inverse of transform)

inline Eigen::Matrix4f getTransform(const Component::Transform &t) {
  Eigen::Matrix3f R = t.rotation.toRotationMatrix();
  Eigen::Matrix3f S = t.scale.asDiagonal();

  Eigen::Matrix4f M = Eigen::Matrix4f::Identity();
  M.block<3, 3>(0, 0) = R * S;
  M.block<3, 1>(0, 3) = t.translation;

  return M;
}

inline Eigen::Matrix4f getInvTransform(const Component::Transform &t) {
  Eigen::Matrix3f R = t.rotation.toRotationMatrix();
  Eigen::Matrix3f S_inv = t.scale.cwiseInverse().asDiagonal();
  Eigen::Vector3f T = t.translation;

  Eigen::Matrix4f inv = Eigen::Matrix4f::Identity();
  inv.block<3, 3>(0, 0) = S_inv * R.transpose();
  inv.block<3, 1>(0, 3) = -inv.block<3, 3>(0, 0) * T;

  return inv;
}

} // namespace Perceptral::Math
