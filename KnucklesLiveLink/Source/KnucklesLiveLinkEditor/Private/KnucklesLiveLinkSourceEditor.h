#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "IMessageContext.h"
#include "MessageEndpoint.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Misc/Guid.h"

class SKnucklesLiveLinkSourceEditor : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SKnucklesLiveLinkSourceEditor)
	{
	}

	SLATE_END_ARGS()
	
	~SKnucklesLiveLinkSourceEditor();
	void Construct(const FArguments& Args);
};