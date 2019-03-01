#include "KnucklesLiveLinkSourceFactory.h"
#include "KnucklesLiveLinkSource.h"
#include "KnucklesLiveLinkSourceEditor.h"

#include "ILiveLinkClient.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "KnucklesLiveLinkSourceFactory"

FText UKnucklesLiveLinkSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "Knuckles LiveLink");
}

FText UKnucklesLiveLinkSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Creates a connection to the Knuckles Skeletal Animation Data Stream");
}

TSharedPtr<SWidget> UKnucklesLiveLinkSourceFactory::CreateSourceCreationPanel()
{
	if (!ActiveSourceEditor.IsValid())
	{
		SAssignNew(ActiveSourceEditor, SKnucklesLiveLinkSourceEditor);
	}
	return ActiveSourceEditor;
}

TSharedPtr<ILiveLinkSource> UKnucklesLiveLinkSourceFactory::OnSourceCreationPanelClosed(bool bMakeSource)
{
	//Clean up
	TSharedPtr<FKnucklesLiveLinkSource> NewSource = nullptr;

	if (bMakeSource && ActiveSourceEditor.IsValid())
	{
		NewSource = MakeShared<FKnucklesLiveLinkSource>();
	}

	ActiveSourceEditor = nullptr;
	return NewSource;
}
#undef LOCTEXT_NAMESPACE