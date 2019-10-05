#pragma once

struct Vertex_PCU;
class Entity;
class Camera;

void DrawDisc( const Vertex_PCU translation, float radius );
float GetDistanceBetween( const Entity* entityA, const Entity* entiryB );
float GetRandomlyChosenFloat( float a, float b );


