<?php

/**
 * @file
 * Add a token which contains the shopping cart view.
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

  // Define the token for the shopping cart.
  $order = array();
  $order['abandoned-cart-view'] = array(
    'name' => t('Abandoned cart content', array(), array('context' => 'a drupal commerce order')),
    'description' => t('The content of the cart from the abandoned_cart_token view.'),
  );
  $order['first-product-line-item'] = array(
    'name' => t('First product line item in the order'),
    'description' => t('The first line item in the order that represents a product (as opposed to shipping, coupons, etc.).'),
    'type' => 'commerce-line-item',
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
        case 'abandoned-cart-view':
          // Load the view that we use to fill the token.
          $view_name = 'abandoned_cart_token';
          $display_name = 'abandoned_cart_token_block';
          $shopping_cart_view = views_embed_view($view_name, $display_name, $order->order_id);

          $replacements[$original] = $shopping_cart_view;

          break;
      }
    }

    if ($line_item_tokens = token_find_with_prefix($tokens, 'first-product-line-item')) {
      $line_item = commerce_abandoned_cart_notification_first_product_line_item($order);
      if ($line_item) {
        $replacements += token_generate('commerce-line-item', $line_item_tokens, array('commerce-line-item' => $line_item), $options);
      }
    }
  }

  return $replacements;
}

/**
 * Retrieve the first line item of an order that references a product.
 */
function commerce_abandoned_cart_notification_first_product_line_item($order) {
  $ids = array_map('_commerce_abandoned_cart_notification_order_line_item_ids', $order->commerce_line_items[LANGUAGE_NONE]);
  $line_items = commerce_line_item_load_multiple($ids);
  foreach ($line_items as $line_item) {
    if ($line_item->type == 'product') {
      return $line_item;
    }
  }
  return FALSE;
}

function _commerce_abandoned_cart_notification_order_line_item_ids($line_item) {
  return $line_item['line_item_id'];
}
