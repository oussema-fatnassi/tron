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

    // Calculate forward vector (from eye to target)
    float forwardX = targetX - eyeX;
    float forwardY = targetY - eyeY;
    float forwardZ = targetZ - eyeZ;
    float forwardLength = sqrtf(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;

    // Calculate right vector = up × forward (cross product)
    float rightX = upY * forwardZ - upZ * forwardY;
    float rightY = upZ * forwardX - upX * forwardZ;
    float rightZ = upX * forwardY - upY * forwardX;
    float rightLength = sqrtf(rightX * rightX + rightY * rightY + rightZ * rightZ);
    rightX /= rightLength;
    rightY /= rightLength;
    rightZ /= rightLength;

    // Calculate actual up vector = forward × right
    float actualUpX = forwardY * rightZ - forwardZ * rightY;
    float actualUpY = forwardZ * rightX - forwardX * rightZ;
    float actualUpZ = forwardX * rightY - forwardY * rightX;

    Matrix result;

    // For a view matrix in a right-handed system:
    // We need to create the INVERSE of the camera transformation
    // The rotation part is the transpose of the camera basis
    // The translation part is -R^T * eye

    // Fill the matrix in COLUMN-MAJOR order
    // Column 0: Right vector
    result.m[0] = rightX;
    result.m[1] = actualUpX;
    result.m[2] = -forwardX;
    result.m[3] = 0.0f;

    // Column 1: Up vector  
    result.m[4] = rightY;
    result.m[5] = actualUpY;
    result.m[6] = -forwardY;
    result.m[7] = 0.0f;

    // Column 2: Forward vector (negated for right-handed)
    result.m[8] = rightZ;
    result.m[9] = actualUpZ;
    result.m[10] = -forwardZ;
    result.m[11] = 0.0f;

    // Column 3: Translation
    // This is -R^T * eye where R^T is already in the matrix
    result.m[12] = -(rightX * eyeX + rightY * eyeY + rightZ * eyeZ);
    result.m[13] = -(actualUpX * eyeX + actualUpY * eyeY + actualUpZ * eyeZ);
    result.m[14] = -(-forwardX * eyeX + -forwardY * eyeY + -forwardZ * eyeZ);
    result.m[15] = 1.0f;

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
    Matrix result;
    float det;
    float inv[16];
    const float* m = this->m;

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + 
             m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - 
             m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + 
             m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - 
              m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - 
             m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + 
             m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - 
             m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + 
              m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + 
             m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - 
             m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + 
              m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - 
              m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - 
             m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - 
              m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) {
        std::cout << "[Matrix] Warning: Inverse of singular matrix requested, returning identity\n";
        return Identity();
    }

    det = 1.0f / det;

    for (int i = 0; i < 16; i++) {
        result.m[i] = inv[i] * det;
    }

    return result;
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