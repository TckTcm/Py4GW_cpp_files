#include "py_trading.h"

PYBIND11_EMBEDDED_MODULE(PyTrading, m)
{
	py::class_<Trade>(m, "PyTrading")
		.def_static("OpenTradeWindow", &Trade::OpenTradeWindow, "Open a trade window with a specific agent", py::arg("agent_id"))
		.def_static("AcceptTrade", &Trade::AcceptTrade, "Accept the current trade")
		.def_static("CancelTrade", &Trade::CancelTrade, "Cancel the current trade")
		.def_static("ChangeOffer", &Trade::ChangeOffer, "Change the current trade offer")
		.def_static("SubmitOffer", &Trade::SubmitOffer, "Submit the current trade offer with specified gold amount", py::arg("gold"))
		.def_static("RemoveItem", &Trade::RemoveItem, "Remove an item from the trade offer by slot index", py::arg("slot"))
		.def_static("GetItemOffered", &Trade::GetItemOffered, "Get details of an item offered in the trade by item ID", py::arg("item_id"))
		.def_static("IsItemOffered", &Trade::IsItemOffered, "Check if an item is offered in the trade by item ID", py::arg("item_id"))
		.def_static("OfferItem", &Trade::OfferItem, "Offer an item for trade by item ID and optional quantity", py::arg("item_id"), py::arg("quantity") = 0)
		.def_static("IsTradeOffered", &Trade::IsTradeOffered, "Check if a trade is currently offered")
		.def_static("IsTradeInitiated", &Trade::IsTradeInitiated, "Check if a trade has been initiated")
		.def_static("IsTradeAccepted", &Trade::IsTradeAccepted, "Check if a trade has been accepted");
}