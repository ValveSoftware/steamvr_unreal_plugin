#pragma once

#include "LiveLinkSourceFactory.h"
#include "KnucklesLiveLinkSourceFactory.generated.h"

class SKnucklesLiveLinkSourceEditor;

UCLASS()
class UKnucklesLiveLinkSourceFactory : public ULiveLinkSourceFactory
{
public:

	GENERATED_BODY()

	virtual FText GetSourceDisplayName() const;
	virtual FText GetSourceTooltip() const;

	virtual TSharedPtr<SWidget> CreateSourceCreationPanel();
	virtual TSharedPtr<ILiveLinkSource> OnSourceCreationPanelClosed(bool bMakeSource);

	TSharedPtr<SKnucklesLiveLinkSourceEditor> ActiveSourceEditor;	
};