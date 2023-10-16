#include "cbridge_plugin.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void print_chain_id(uint32_t chainId, char *destination, size_t max_length) {
  snprintf(destination, max_length, "%d", chainId);
}

// Set UI for the "Send" screen.
static bool set_send_ui(ethQueryContractUI_t *msg, const context_t *context) {
  strlcpy(msg->title, "Send", msg->titleLength);

  uint8_t decimals = context->decimals;
  const char *ticker = context->ticker;

  // If the token look up failed, use the default network ticker along with the
  // default decimals.
  if (!context->token_found) {
    decimals = WEI_TO_ETHER;
    ticker = msg->network_ticker;
  }

  return amountToString(context->amount_in, sizeof(context->amount_in),
                        decimals, ticker, msg->msg, msg->msgLength);
}

static bool set_send_native_ui(ethQueryContractUI_t *msg,
                               const context_t *context) {
  strlcpy(msg->title, "Send", msg->titleLength);

  const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
  uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

  // Converts the uint256 number located in `eth_amount` to its string
  // representation and copies this to `msg->msg`.
  return amountToString(eth_amount, eth_amount_size, WEI_TO_ETHER, "ETH",
                        msg->msg, msg->msgLength);
}

// Set UI for "Receive" screen.
static bool set_receive_ui(ethQueryContractUI_t *msg,
                           const context_t *context) {
  strlcpy(msg->title, "Destination Chain.", msg->titleLength);

  print_chain_id(context->dst_chain_id, msg->msg, msg->msgLength);
  return true;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
  context_t *context = (context_t *)msg->pluginContext;
  bool ret = false;

  // msg->title is the upper line displayed on the device.
  // msg->msg is the lower line displayed on the device.

  // Clean the display fields.
  memset(msg->title, 0, msg->titleLength);
  memset(msg->msg, 0, msg->msgLength);

  // Adapt the cases for the screens you'd like to display.

  if (context->selectorIndex == POOL_BASED_SEND_ERC20) {
    switch (msg->screenIndex) {
    case 0:
      ret = set_send_ui(msg, context);
      break;
    case 1:
      ret = set_receive_ui(msg, context);
      break;
    // Keep this
    default:
      PRINTF("Received an invalid screenIndex\n");
    }
  } else if (context->selectorIndex == POOL_BASED_SEND_NATIVE) {
    switch (msg->screenIndex) {
    case 0:
      ret = set_send_native_ui(msg, context);
      break;
    case 1:
      ret = set_receive_ui(msg, context);
      break;
    // Keep this
    default:
      PRINTF("Received an invalid screenIndex\n");
    }
  } else if (context->selectorIndex == PEGGED_TOKEN_DEPOSIT_MINT) {
    switch (msg->screenIndex) {
    case 0:
      ret = set_send_ui(msg, context);
      break;
    case 1:
      ret = set_receive_ui(msg, context);
      break;
    // Keep this
    default:
      break;
    }
  } else if (context->selectorIndex == PEGGED_TOKEN_BURN_WITHDRAW) {
    switch (msg->screenIndex) {
    case 0:
      ret = set_send_ui(msg, context);
      break;
    default:
      break;
    }
  }
  msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
