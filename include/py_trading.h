#pragma once
#include "Headers.h"

class Trade {
public:
	static void OpenTradeWindow(uint32_t agent_id) {
		GW::GameThread::Enqueue([agent_id] {
			GW::Trade::OpenTradeWindow(agent_id);
			});
	}

	static bool AcceptTrade() {
		return GW::Trade::AcceptTrade();
	}
	static bool CancelTrade() {
		return GW::Trade::CancelTrade();
	}
	static bool ChangeOffer() {
		return GW::Trade::ChangeOffer();
	}
	static bool SubmitOffer(uint32_t gold) {
		return GW::Trade::SubmitOffer(gold);
	}
	static bool RemoveItem(uint32_t slot) {
		return GW::Trade::RemoveItem(slot);
	}

	static GW::TradeItem* GetItemOffered(uint32_t item_id) {
		return GW::Trade::IsItemOffered(item_id);
	}

	static bool IsItemOffered(uint32_t item_id) {
		return GW::Trade::IsItemOffered(item_id) != nullptr;
	}

	static bool OfferItem(uint32_t item_id, uint32_t quantity = 0) {
		return GW::Trade::OfferItem(item_id, quantity);
	}

	static bool IsTradeOffered() {
		const auto ctx = GW::GetTradeContext();
		if (!ctx) return false;
		return ctx->GetIsTradeOffered();
	}

	static bool IsTradeInitiated() {
		const auto ctx = GW::GetTradeContext();
		if (!ctx) return false;
		return ctx->GetIsTradeInitiated();
	}

	static bool IsTradeAccepted() {
		const auto ctx = GW::GetTradeContext();
		if (!ctx) return false;
		return ctx->GetIsTradeAccepted();
	}
};