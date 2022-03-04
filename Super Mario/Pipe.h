#pragma once

typedef struct Pipe
{
	Object object;
	Location location;
	ChildImage id;
}Pipe;

Pipe * PipeConstructor(Game * game,ChildImage id);