Feature: Internet shop OpenCart; User Experience


  Scenario: User registration
    Given User is prompted to register
    When User fills required personal information
    Then User's account has been created

  Scenario: Invalid information access
    Given Unregistered user is on login page
    When Unregistered user clicks account specific tabs
    Then Unregistered user will not access these tabs

  Scenario: Users information edit
    Given User is on Account Information tab
    When User changes information
    Then Information are updated

  Scenario: Users enquiry
    Given User's selected Contact Us tab
    When User submits enquiry
    Then User is prompted about successful submission

  Scenario: Password change
    Given User has changed account password
    When User tries to log in
    Then New password will work instead of old

  Scenario: Forgotten password no.1
    Given User is on Forgotten Password page
    When User fills valid email address and continues
    Then User is informed that confirmation link has been sent

  Scenario: Forgotten password no.2
    Given User is on Forgotten Password page
    When User fills invalid email address and continues
    Then User is informed that operation failed and email was invalid

  Scenario: Address removal no.1
    Given User is on Address Book Entries page with address entries
    When User clicks Delete button
    Then Address entry is not shown anymore

  Scenario: Address removal no.2
    Given User is on Address Book Entries page with defaulted address
    When User clicks Delete button on defaulted address
    Then User is prompted that he cannot delete this address

  Scenario: Affiliate tracking link
    Given User has affiliate account
    When User chooses item for tracking
    Then Site generates link that directs user to item's page

  Scenario: Subscription change
    Given User is on Newsletter Subscription page
    When User changes subscribe option and continues
    Then User receives feedback about subscription update

  Scenario: User logout
    Given User is logged in and on My Account page
    When User clicks Logout
    Then User is redirected from current page
    And User is prompted about logout change
    And User may login with credentials now

  Scenario: Preserve Cart Items
    Given User has added items in Cart and is not logged in
    When User logs in
    Then Items in cart are preserved

  Scenario: Return information
    Given User ordered item and is on Order Information page
    When User clicks Return button, fills information and submits
    Then Item will be shown in Product Returns page

  Scenario: Wish list add
    Given User is on item page
    When User clicks Add to Wish list
    Then Item will be shown in Wish list

  Scenario: Wish list remove
    Given User is on Wish List page with added item
    When User clicks Remove
    Then User is prompted about change
    And Item is no longer shown in Wish List

  Scenario: Invalid shopping cart
    Given User is on Cart page with items
    When User exceeds item's stock quantity and updates cart
    Then User is prompted about items's inavailbility

  Scenario: Valid checkout
    Given Logged user is on Checkout page while having item(s) in cart
    When User proceeds through Checkout Steps and click on Confirm Order
    Then User is prompted about success of operation
    And  Order is shown on Order History page

  Scenario: Invalid checkout no.1
    Given User ordered unavailable item
    When User clicks checkout
    Then User is prompted about item's unavailability
    And Checkout is not initiated

  Scenario: Invalid checkout no.2
    Given User has empty Shopping cart
    When User clicks Checkout
    Then User is prompted about empty cart
    And Checkout is not initiated

  Scenario: Invalid checkout no.3
    Given User is on the last checkout step
    When User removes items from cart
    Then Checkout is cancelled
