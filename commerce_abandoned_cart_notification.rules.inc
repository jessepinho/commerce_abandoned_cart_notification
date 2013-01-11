<?php

/**
 * @file
 * commerce_abandoned_cart_notification.rules_defaults.inc
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_abandoned_cart_notification_rules_action_info() {
  return array(
    'commerce_abandoned_cart_notification_fetch_orders_action' => array(
      'label' => 'Fetch abandoned orders',
      'group' => t('Commerce Order'),
      'parameter' => array(
        'threshold' => array(
          'label' => t('Abandonment threshold'),
          'description' => t('After how long should an order be considered abandoned?'),
          'type' => 'integer',
          'options list' => 'commerce_abandoned_cart_notification_time_options',
          'default value' => 60 * 60 * 24,
        ),
        'statuses' => array(
          'label' => t('Statuses'),
          'description' => t('Only orders with these statuses will be eligible for notifications.'),
          'type' => 'list<text>',
          'options list' => 'commerce_order_status_options_list',
        ),
        'limit' => array(
          'label' => t('Notification limit'),
          'description' => t('After this many notifications have been sent, notifications will cease.'),
          'type' => 'integer',
        ),
        'delay' => array(
          'label' => t('Notification delay'),
          'description' => t('How much of a delay should there be between notifications? (This is only applicable if "Notification limit" above is greater than 1.)'),
          'type' => 'integer',
          'options list' => 'commerce_abandoned_cart_notification_time_options',
          'default value' => 60 * 60 * 24 * 7,
          'optional' => TRUE,
        ),
      ),
      'provides' => array(
        'orders' => array(
          'label' => t('Abandoned orders'),
          'type' => 'list<commerce_order>',
        ),
      ),
    ),
    'commerce_abandoned_cart_notification_log_notification_action' => array(
      'label' => 'Log an abandoned order notification',
      'group' => t('Commerce Order'),
      'parameter' => array(
        'changed' => array(
          'label' => t('Order'),
          'description' => t('The order for which this notification was sent.'),
          'type' => 'commerce_order',
        ),
      ),
    ),
  );
}

/**
 * Fetch all orders with the given specifications.
 */
function commerce_abandoned_cart_notification_fetch_orders_action($threshold, $statuses, $limit, $delay) {
  // Initiate the return value.
  $return = array('orders' => array());

  $query = db_query('
    SELECT
      co.order_id,
      COUNT(cacn.id) AS message_count,
      MAX(cacn.created) AS last_message
    FROM
      {commerce_order} co
      LEFT JOIN {commerce_abandoned_cart_notification} cacn
        ON cacn.order_id = co.order_id
    WHERE
      co.changed <= :changed
      AND co.status IN (:statuses)
    GROUP BY cacn.order_id
    HAVING message_count < :limit
      AND
        (last_message <= :delay
        OR last_message IS NULL)
    ORDER BY co.changed ASC
  ', array(
    ':changed' => REQUEST_TIME - $threshold,
    ':statuses' => $statuses,
    ':delay' => REQUEST_TIME - $delay,
    ':limit' => $limit,
  ));
  $result = $query->fetchAll();
  if (!empty($result)) {
    $ids = array_map(function($row) {
      return $row->order_id;
    }, $result);
    $return['orders'] = commerce_order_load_multiple($ids);
  }
  return $return;
}

/**
 * Log the message we sent to the user.
 */
function commerce_abandoned_cart_notification_log_notification_action($order) {
  // Create the notification.
  $n = array(
    'uid' => $order->uid,
    'order_id' => $order->order_id,
    'order_vid' => $order->revision_id,
    'created' => REQUEST_TIME,
    'data' => array(),
  );
  drupal_write_record('commerce_abandoned_cart_notification', $n);
}

/**
 * Return options for the notification delay.
 */
function commerce_abandoned_cart_notification_time_options() {
  return array(
    60 * 10 => t('10 minutes'),
    60 * 15 => t('15 minutes'),
    60 * 30 => t('30 minutes'),
    60 * 45 => t('45 minutes'),
    60 * 60 => t('1 hour'),
    60 * 60 * 2 => t('2 hours'),
    60 * 60 * 6 => t('6 hours'),
    60 * 60 * 12 => t('12 hours'),
    60 * 60 * 24 => t('1 day'),
    60 * 60 * 24 * 7 => t('1 week'),
    60 * 60 * 24 * 30 => t('30 days'),
  );
}
