<?php

/**
 * @file
 * commerce_abandoned_cart_notification.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_abandoned_cart_notification_default_rules_configuration() {
  $items = array();
  $items['rules_notify_user_abandoned_cart'] = entity_import('rules_config', '
  { "rules_notify_user_abandoned_cart" : {
    "LABEL" : "Notify the user of his abandoned cart",
    "PLUGIN" : "rule set",
    "REQUIRES" : [ "rules", "commerce_order" ],
    "USES VARIABLES" : { "abandoned_cart" : { "label" : "Abandoned Cart", "type" : "commerce_order" } },
    "RULES" : [
      { "RULE" : {
          "IF" : [
            { "NOT data_is_empty" : { "data" : [ "abandoned-cart:mail" ] } },
            { "NOT data_is" : { "data" : [ "abandoned-cart:status" ], "value" : "user_notified" } }
          ],
          "DO" : [
            { "mail" : {
                "to" : "[abandoned-cart:mail]",
                "subject" : "[site:name] - Reminder of your shopping cart",
                "message" : "Dear customer,\r\n\r\nYou can find your shopping cart here : [abandoned-cart:customer-url]\r\n\r\n[abandoned-cart:order-shopping-cart-table]\r\n\r\nSee you soon online !"
              }
            },
            { "commerce_order_update_status" : {
                "commerce_order" : [ "abandoned-cart" ],
                "order_status" : "user_notified"
              }
            },
            { "drupal_message" : { "message" : "Successfully sent e-mail to [abandoned-cart:mail]." } }
          ],
          "LABEL" : "Send e-mail to the user with the content of his abandoned cart"
        }
      },
      { "RULE" : {
          "IF" : [ { "data_is_empty" : { "data" : [ "abandoned-cart:mail" ] } } ],
          "DO" : [
            { "drupal_message" : {
                "message" : "No e-mail attached to the shopping cart n\u00b0[abandoned-cart:order-id].",
                "type" : "warning"
              }
            }
          ],
          "LABEL" : "Display a message if no e-mail address is attached to the cart."
        }
      },
      { "RULE" : {
          "IF" : [
            { "data_is" : { "data" : [ "abandoned-cart:status" ], "value" : "user_notified" } }
          ],
          "DO" : [
            { "drupal_message" : {
                "message" : "A notification has already been sent to [abandoned-cart:mail].",
                "type" : "warning"
              }
            }
          ],
          "LABEL" : "Display a message if the notification has already been sent"
        }
      }
    ]
  }
}
');
  $items['rules_commerce_abandoned_cart_notification_cron'] = entity_import('rules_config', '
  { "rules_commerce_abandoned_cart_notification_cron" : {
    "LABEL" : "Cron - send abandoned order notification",
    "PLUGIN" : "rule",
    "REQUIRES" : [ "rules", "commerce_abandoned_cart_notification" ],
    "USES VARIABLES" : {
      "abandoned_order" : { "label" : "Abandoned order", "type" : "commerce_order" },
      "subject" : { "label" : "Subject", "type" : "text" },
      "message" : { "label" : "Message", "type" : "text" },
      "from" : { "label" : "From", "type" : "text" }
    },
    "IF" : [ { "NOT data_is_empty" : { "data" : [ "abandoned-order:mail" ] } } ],
    "DO" : [
      { "commerce_abandoned_cart_notification_log_notification_action" : { "changed" : [ "abandoned-order" ] } },
      { "mail" : {
          "to" : [ "abandoned-order:mail" ],
          "subject" : [ "subject" ],
          "message" : [ "message" ],
          "from" : [ "from" ],
          "language" : [ "" ]
        }
      }
    ]
  }
}
');
  $items['rules_commerce_abandoned_cart_notifications_cron'] = entity_import('rules_config', '
  { "rules_commerce_abandoned_cart_notifications_cron" : {
    "LABEL" : "Cron - send abandoned order notifications",
    "PLUGIN" : "reaction rule",
    "REQUIRES" : [ "commerce_abandoned_cart_notification", "rules" ],
    "ON" : [ "cron" ],
    "DO" : [
      { "commerce_abandoned_cart_notification_fetch_orders_action" : {
          "USING" : {
            "threshold" : "86400",
            "statuses" : { "value" : {
                "cart" : "cart",
                "checkout_checkout" : "checkout_checkout",
                "checkout_review" : "checkout_review",
                "checkout_payment" : "checkout_payment"
              }
            },
            "limit" : "1"
          },
          "PROVIDE" : { "orders" : { "orders" : "Abandoned orders" } }
        }
      },
      { "LOOP" : {
          "USING" : { "list" : [ "orders" ] },
          "ITEM" : { "order" : "Order" },
          "DO" : [
            { "component_rules_commerce_abandoned_cart_notification_cron" : {
                "abandoned_order" : [ "order" ],
                "subject" : "[site:name] - Your order",
                "message" : "Hi! Your order #[order:order-id] is waiting for you to check out! Please visit [site:url]checkout to complete your order.",
                "from" : [ "site:mail" ]
              }
            }
          ]
        }
      }
    ]
  }
}
');
  return $items;
}
