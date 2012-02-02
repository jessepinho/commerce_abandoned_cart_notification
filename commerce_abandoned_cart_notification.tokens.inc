<?php

/**
 * @file
 * Builds placeholder replacement tokens for order-related data.
 */

/**
 * Implements hook_token_info().
 */
function commerce_abandoned_cart_notification_token_info() {
  $type = array(
      'name' => t('Orders', array(), array('context' => 'a drupal commerce order')),
      'description' => t('Tokens related to individual orders.'),
      'needs-data' => 'commerce-order',
  );

  // Tokens for orders.
  $order = array();

  $order['order-shopping-cart-table'] = array(
      'name' => t('Order content', array(), array('context' => 'a drupal commerce order')),
      'description' => t('The content of the shopping cart as an HTML array.'),
  );

  return array(
      'types' => array('commerce-order' => $type),
      'tokens' => array('commerce-order' => $order),
  );
}

/**
 * Implements hook_tokens().
 */
function commerce_abandoned_cart_notification_tokens($type, $tokens, array $data = array(), array $options = array()) {

  $replacements = array();

  if ($type == 'commerce-order' && !empty($data['commerce-order'])) {
    $order = $data['commerce-order'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Simple key values on the order.
        case 'order-shopping-cart-table':
          $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

          // Build the shopping cart array token
          $shopping_cart_array = array();
          foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
            $shopping_cart_array['line-item-' . $line_item_wrapper->line_item_id->value()] = array(
                'label' => $line_item_wrapper->line_item_label->value(),
                'quantity' => $line_item_wrapper->quantity->value(),
                'amount' => $line_item_wrapper->commerce_total->amount->value(),
            );
          }

          // Generate the HTML content from the array
          $header = array(t('Label'), t('Quantity'), t('Amount'));
          $rows = array();
          foreach ($shopping_cart_array as $data => $line_item) {
            $rows[] = array($line_item['label'], $line_item['quantity'], $line_item['amount']);
          }

          $shopping_cart_table = theme('table', array('header' => $header, 'rows' => $rows));

          $replacements[$original] = $shopping_cart_table;

          break;
      }
    }
  }

  return $replacements;
}