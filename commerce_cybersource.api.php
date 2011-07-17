<?php

/**
 * @file
 * Hook documentation for the Commerce CyberSource module.
 */


/**
 * Allows modules to alter an item about to be added to an order when CyberSource
 * is configured to send itemized orders for processing.
 *
 * @param $item
 *   The item object representing a line item on the order.
 * @param $line_item
 *   The line item object represented by the item.
 */
function hook_commerce_cybersource_item_alter($item, $line_item) {
  // No example.
}
