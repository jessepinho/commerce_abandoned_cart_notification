<?php

/**
 * @file
 * commerce_abandoned_cart_notification.rules_defaults.inc
 */

/**
 * Implementation of hook_default_rules_configuration().
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
                "subject" : "[site:name] - Reminder of your cart",
                "message" : "Hi,\r\n\r\nYou have a waiting shopping cart that you can find here : [abandoned-cart:customer-url]\r\n\r\nSee you soon online !"
              }
            },
            { "commerce_order_update_status" : {
                "commerce_order" : [ "abandoned-cart" ],
                "order_status" : "user_notified"
              }
            },
            { "drupal_message" : { "message" : "Successfully reminds [abandoned-cart:mail] of his abandoned cart." } }
          ],
          "LABEL" : "Send e-mail to the user with the content of his abandoned cart"
        }
      },
      { "RULE" : {
          "IF" : [ { "data_is_empty" : { "data" : [ "abandoned-cart:mail" ] } } ],
          "DO" : [
            { "drupal_message" : {
                "message" : "No e-mail found for the shopping cart n\u00b0[abandoned-cart:order-id].",
                "type" : "warning"
              }
            }
          ],
          "LABEL" : "Display a message if the user has no e-mail address."
        }
      },
      { "RULE" : {
          "IF" : [
            { "data_is" : { "data" : [ "abandoned-cart:status" ], "value" : "user_notified" } }
          ],
          "DO" : [
            { "drupal_message" : {
                "message" : "A message has already been sent to [abandoned-cart:mail] for his abandoned cart.",
                "type" : "warning"
              }
            }
          ],
          "LABEL" : "Display a message if the reminder has already been sent"
        }
      }
    ]
  }
}  
');
  return $items;
}
