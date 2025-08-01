#include "../../Headers/Math/Matrix.hpp"
#include <cstring>
#include <iomanip>

Matrix::Matrix() {
    SetIdentity();
}

Matrix::Matrix(const float* data) {
    memcpy(m, data, sizeof(float) * 16);
}

Matrix::Matrix(const Matrix& other) {
    memcpy(m, other.m, sizeof(float) * 16);
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        memcpy(m, other.m, sizeof(float) * 16);
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result;

    // Column-major multiplication
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += At(row, k) * other.At(k, col);
            }
            result.At(row, col) = sum;
        }
    }

    return result;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    *this = *this * other;
    return *this;
}

void Matrix::SetIdentity() {
    memset(m, 0, sizeof(float) * 16);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Matrix Matrix::Identity() {
    Matrix result;
    result.SetIdentity();
    return result;
}

Matrix Matrix::Translation(float x, float y, float z) {
    Matrix result;
    result.At(0, 3) = x;  // m[12]
    result.At(1, 3) = y;  // m[13]
    result.At(2, 3) = z;  // m[14]
    return result;
}

Matrix Matrix::RotationX(float angleRadians) {
    Matrix result;
    float cos_a = cosf(angleRadians);
    float sin_a = sinf(angleRadians);

    result.At(1, 1) = cos_a;   // m[5]
    result.At(1, 2) = -sin_a;  // m[9]
    result.At(2, 1) = sin_a;   // m[6]
    result.At(2, 2) = cos_a;   // m[10]

    return result;
}

Matrix Matrix::RotationY(float angleRadians) {
    Matrix result;
    float cos_a = cosf(angleRadians);
    float sin_a = sinf(angleRadians);

    result.At(0, 0) = cos_a;   // m[0]
    result.At(0, 2) = sin_a;   // m[8]
    result.At(2, 0) = -sin_a;  // m[2]
    result.At(2, 2) = cos_a;   // m[10]

    return result;
}

Matrix Matrix::RotationZ(float angleRadians) {
    Matrix result;
    float cos_a = cosf(angleRadians);
    float sin_a = sinf(angleRadians);

    result.At(0, 0) = cos_a;   // m[0]
    result.At(0, 1) = -sin_a;  // m[4]
    result.At(1, 0) = sin_a;   // m[1]
    result.At(1, 1) = cos_a;   // m[5]

    return result;
}

Matrix Matrix::RotationEuler(float pitch, float yaw, float roll) {
    // Apply rotations in order: Roll (Z) * Pitch (X) * Yaw (Y)
    return RotationZ(roll) * RotationX(pitch) * RotationY(yaw);
}

Matrix Matrix::Scale(float x, float y, float z) {
    Matrix result;
    result.At(0, 0) = x;   // m[0]
    result.At(1, 1) = y;   // m[5]
    result.At(2, 2) = z;   // m[10]
    return result;
}

Matrix Matrix::Scale(float uniform) {
    return Scale(uniform, uniform, uniform);
}

Matrix Matrix::LookAt(float eyeX, float eyeY, float eyeZ,
    float targetX, float targetY, float targetZ,
    float upX, float upY, float upZ) {
    // Calculate forward vector (normalized)
    float forwardX = targetX - eyeX;
    float forwardY = targetY - eyeY;
    float forwardZ = targetZ - eyeZ;
    float forwardLength = sqrtf(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;

    // Calculate right vector (forward x up, normalized)
    float rightX = forwardY * upZ - forwardZ * upY;
    float rightY = forwardZ * upX - forwardX * upZ;
    float rightZ = forwardX * upY - forwardY * upX;
    float rightLength = sqrtf(rightX * rightX + rightY * rightY + rightZ * rightZ);
    rightX /= rightLength;
    rightY /= rightLength;
    rightZ /= rightLength;

    // Calculate true up vector (right x forward)
    float trueUpX = rightY * forwardZ - rightZ * forwardY;
    float trueUpY = rightZ * forwardX - rightX * forwardZ;
    float trueUpZ = rightX * forwardY - rightY * forwardX;

    Matrix result;
    // Set rotation part
    result.At(0, 0) = rightX;   result.At(0, 1) = trueUpX;   result.At(0, 2) = -forwardX;
    result.At(1, 0) = rightY;   result.At(1, 1) = trueUpY;   result.At(1, 2) = -forwardY;
    result.At(2, 0) = rightZ;   result.At(2, 1) = trueUpZ;   result.At(2, 2) = -forwardZ;

    // Set translation part (negative dot products)
    result.At(0, 3) = -(rightX * eyeX + rightY * eyeY + rightZ * eyeZ);
    result.At(1, 3) = -(trueUpX * eyeX + trueUpY * eyeY + trueUpZ * eyeZ);
    result.At(2, 3) = forwardX * eyeX + forwardY * eyeY + forwardZ * eyeZ;

    return result;
}

Matrix Matrix::Perspective(float fovYRadians, float aspectRatio, float nearPlane, float farPlane) {
    Matrix result;
    memset(result.m, 0, sizeof(float) * 16);

    float tanHalfFovY = tanf(fovYRadians * 0.5f);

    result.At(0, 0) = 1.0f / (aspectRatio * tanHalfFovY);
    result.At(1, 1) = 1.0f / tanHalfFovY;
    result.At(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.At(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    result.At(3, 2) = -1.0f;

    return result;
}

Matrix Matrix::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    Matrix result;

    result.At(0, 0) = 2.0f / (right - left);
    result.At(1, 1) = 2.0f / (top - bottom);
    result.At(2, 2) = -2.0f / (farPlane - nearPlane);
    result.At(0, 3) = -(right + left) / (right - left);
    result.At(1, 3) = -(top + bottom) / (top - bottom);
    result.At(2, 3) = -(farPlane + nearPlane) / (farPlane - nearPlane);

    return result;
}

Matrix Matrix::Transpose() const {
    Matrix result;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result.At(col, row) = At(row, col);
        }
    }
    return result;
}

Matrix Matrix::Inverse() const {
    // TODO: Implement proper matrix inversion
    // For now, return identity as placeholder
    std::cout << "[Matrix] Warning: Inverse() not implemented, returning identity\n";
    return Identity();
}

void Matrix::Print() const {
    std::cout << "Matrix:\n";
    for (int row = 0; row < 4; row++) {
        std::cout << "  ";
        for (int col = 0; col < 4; col++) {
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << At(row, col);
        }
        std::cout << "\n";
    }
}