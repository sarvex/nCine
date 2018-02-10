#include "nctl/algorithms.h"
#include "RenderQueue.h"
#include "SceneNode.h"
#include "Sprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderQueue::RenderQueue()
	: numVertices_(0), lastNumVertices_(0), lastNumCommands_(0),
	  opaqueRenderCommands_(16), transparentRenderCommands_(16)
{
	for (unsigned int i = 0; i < RenderCommand::CommandType::COUNT; i++)
	{
		typedLastNumVertices_[i] = typedNumVertices_[i];
		typedLastNumCommands_[i] = typedNumCommands_[i];
		typedNumVertices_[i] = 0;
		typedNumCommands_[i] = 0;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderQueue::addCommand(RenderCommand *command)
{
	// Calculating a sorting key before adding the command to the queue
	command->calculateSortKey();

	typedNumVertices_[command->type()] += command->geometry().numVertices();
	typedNumCommands_[command->type()]++;

	numVertices_ += command->geometry().numVertices();

	if (command->material().isTransparent() == false)
		opaqueRenderCommands_.pushBack(command);
	else
		transparentRenderCommands_.pushBack(command);
}

namespace {

bool descendingOrder(const RenderCommand *a, const RenderCommand *b) { return a->sortKey() > b->sortKey(); }
bool ascendingOrder(const RenderCommand *a, const RenderCommand *b) { return a->sortKey() < b->sortKey(); }

}

void RenderQueue::draw()
{
	// Sorting the queues with the relevant orders
	nctl::quicksort(opaqueRenderCommands_.begin(), opaqueRenderCommands_.end(), descendingOrder);
	nctl::quicksort(transparentRenderCommands_.begin(), transparentRenderCommands_.end(), ascendingOrder);

	// Rendering opaque nodes front to back
	for (RenderCommand *opaqueRenderCommand : opaqueRenderCommands_)
		opaqueRenderCommand->issue();

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	// Rendering transparent nodes back to front
	for (RenderCommand *transparentRenderCommand : transparentRenderCommands_)
		transparentRenderCommand->issue();
	// Has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	lastNumVertices_ = numVertices_;
	lastNumCommands_ = opaqueRenderCommands_.size() + transparentRenderCommands_.size();
	numVertices_ = 0;

	for (unsigned int i = 0; i < RenderCommand::CommandType::COUNT; i++)
	{
		typedLastNumVertices_[i] = typedNumVertices_[i];
		typedLastNumCommands_[i] = typedNumCommands_[i];
		typedNumVertices_[i] = 0;
		typedNumCommands_[i] = 0;
	}

	opaqueRenderCommands_.clear();
	transparentRenderCommands_.clear();
}

}
