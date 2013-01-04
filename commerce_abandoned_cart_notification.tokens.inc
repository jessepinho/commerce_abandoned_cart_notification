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
  }

  return $replacements;
}
