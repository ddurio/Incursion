#include "Engine/Renderer/Camera.hpp"


void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight ) {
	m_BottomLeft = bottomLeft;
	m_TopRight = topRight;
}


Vec2 Camera::GetOrthoBottomLeft() const {
	return m_BottomLeft;
}


Vec2 Camera::GetOrthoTopRight() const {
	return m_TopRight;
}


Vec2 Camera::GetDimensions() const {
    float width = m_TopRight.x - m_BottomLeft.x;
    float height = m_TopRight.y - m_BottomLeft.y;
    return Vec2( width, height );
}


Vec2 Camera::GetCenter() const {
    float centerX;
    float centerY;
    GetCenter( centerX, centerY );

    return Vec2( centerX, centerY );
}


void Camera::GetCenter( float& centerX_out, float& centerY_out ) const {
    Vec2 dimensions = GetDimensions();

    centerX_out = m_BottomLeft.x + (0.5f * dimensions.x);
    centerY_out = m_BottomLeft.y + (0.5f * dimensions.y);
}


void Camera::Translate2D( const Vec2& translation2D ) {
    m_BottomLeft += translation2D;
    m_TopRight += translation2D;
}
