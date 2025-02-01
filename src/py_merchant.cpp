#pragma once
#include "py_merchant.h"

namespace py = pybind11;







PYBIND11_EMBEDDED_MODULE(PyMerchant, m) {
    py::class_<Merchant>(m, "PyMerchant")
        // Constructor
        .def(py::init<>())

        // Methods
        .def(py::init<>())
        .def("trader_buy_item", &Merchant::TraderBuyItem, py::arg("item_id"), py::arg("cost"), "Buy an item by model ID and quantity")
        .def("trader_sell_item", &Merchant::TraderSellItem, py::arg("item_id"), py::arg("price"), "Sell an item by model ID and quantity")
        .def("trader_request_quote", &Merchant::TraderRequestQuote, py::arg("item_id"), "Request a price quote for the item by its ID")
        .def("trader_request_sell_quote", &Merchant::TraderRequestSellQuote, py::arg("item_id"), "Request a price quote for the item by its ID")
        .def("merchant_buy_item", &Merchant::MerchantBuyItem, py::arg("item_id"), py::arg("cost"), "Buy an item by model ID and quantity")
        .def("merchant_sell_item", &Merchant::MerchantSellItem, py::arg("item_id"), py::arg("price"), "Sell an item by model ID and quantity")
        .def("crafter_buy_item", &Merchant::CrafterBuyItems, py::arg("item_id"), py::arg("cost"), py::arg("give_item_ids"), py::arg("give_item_quantities"), "Buy an item by model ID and quantity")
        .def("collector_buy_item", &Merchant::CollectorBuyItems, py::arg("item_id"), py::arg("cost"), py::arg("give_item_ids"), py::arg("give_item_quantities"), "Buy an item by model ID and quantity")
        .def("get_trader_item_list", &Merchant::GetTraderItems, "Get the list of merchant items queried")
        .def("get_trader_item_list2", &Merchant::GetTraderItems2, "Get the list of merchant items queried")
        .def("get_merchant_item_list", &Merchant::GetMerchantItems, "Get the list of merchant items queried")
        .def("get_quoted_value", &Merchant::GetQuotedValue, "Get the last quoted price for the item")
        .def("get_quoted_item_id", &Merchant::GetQuotedItemID, "Get the last quoted item ID")
        .def("is_transaction_complete", &Merchant::IsTransactionComplete, "Check if the last transaction was completed")

        .def("update", &Merchant::Update, "Update the Merchant state to be called from Python");
}

