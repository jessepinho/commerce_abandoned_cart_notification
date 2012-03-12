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
  return $items;
}
