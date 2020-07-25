#include "core/CoreUtils.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/node/models/InboundNodeModel.hpp"
#include "ui/node/models/OutboundNodeModel.hpp"
#include "ui/node/models/RuleNodeModel.hpp"
#include "w_RoutesEditor.hpp"

// Do not use const reference here.
// void RouteEditor::ResolveDefaultOutboundTag(const QString original, const QString newTag)
//{
// LOG(MODULE_UI, "Resolving default outbound settings: default=" + defaultOutbound + " original=" + original + " new=" + newTag)
// auto isDefaultChanged = original == defaultOutbound;
////
// isLoading = true;
// defaultOutboundCombo->clear();
////
// for (const auto &out : outbounds) defaultOutboundCombo->addItem(getTag(out.realOutbound));
////
// isLoading = false;
////
// if (!isDefaultChanged)
//{
//    LOG(MODULE_UI, "Default outbound is not changed: retaining: " + defaultOutbound)
//    // Just simply restore the default one.
//    defaultOutboundCombo->setCurrentText(defaultOutbound);
//}
// else if (newTag.isEmpty())
//{
//    LOG(MODULE_UI, "Default outbound is removed, using first key from the outbounds as the default one.")
//
//    // Removed the default one, so set the first one as the default.
//    if (outbounds.isEmpty())
//    {
//        defaultOutbound.clear();
//    }
//    else
//    {
//        defaultOutbound = getTag(outbounds.first().realOutbound);
//        defaultOutboundCombo->addItem(getTag(outbounds.first().realOutbound));
//    }
//}
// else
//{
//    LOG(MODULE_UI, "Default outbound is renamed, new tag is " + newTag)
//    defaultOutboundCombo->setCurrentText(newTag);
//    defaultOutbound = newTag;
//}
//}
