#include "cbridge_plugin.h"
#include "plugin_utils.h"

// Called once to init.
void handle_init_contract(ethPluginInitContract_t *msg) {
  // Make sure we are running a compatible version.
  if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
    // If not the case, return the `UNAVAILABLE` status.
    msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
    return;
  }

  // Double check that the `context_t` struct is not bigger than the maximum
  // size (defined by `msg->pluginContextLength`).
  if (msg->pluginContextLength < sizeof(context_t)) {
    PRINTF("Plugin parameters structure is bigger than allowed size\n");
    msg->result = ETH_PLUGIN_RESULT_ERROR;
    return;
  }

  context_t *context = (context_t *)msg->pluginContext;

  // Initialize the context (to 0).
  memset(context, 0, sizeof(*context));

  uint32_t selector = U4BE(msg->selector, 0);
  if (!find_selector(selector, CBRIDGE_SELECTORS, NUM_SELECTORS,
                     &context->selectorIndex)) {
    msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
    return;
  }

  // Set `next_param` to be the first field we expect to parse.
  switch (context->selectorIndex) {
  case POOL_BASED_SEND_ERC20:
    context->next_param = RECEIVER;
    break;
  case POOL_BASED_SEND_NATIVE:
    context->next_param = RECEIVER;
    break;
  case PEGGED_TOKEN_DEPOSIT_MINT:
    context->next_param = TOKEN;
    break;
  case PEGGED_TOKEN_BURN_WITHDRAW:
    context->next_param = TOKEN;
    break;
  case POOL_BASED_TOKEN_REFUND:
    break;
  // Keep this
  default:
    PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
    msg->result = ETH_PLUGIN_RESULT_ERROR;
    return;
  }

  // Return valid status.
  msg->result = ETH_PLUGIN_RESULT_OK;
}
