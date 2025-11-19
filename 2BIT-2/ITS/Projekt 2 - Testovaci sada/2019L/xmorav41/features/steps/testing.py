import random
import time
import string

from behave import *
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities

letters = string.ascii_lowercase
email_pwd = ''.join(random.choice(letters) for i in range(5))
email_pwd05 = ''.join(random.choice(letters) for i in range(5))

lorem_ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et " \
              "dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut " \
              "aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum " \
              "dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui " \
              "officia deserunt mollit anim id est laborum. "


def reg(context):
    letters = string.ascii_lowercase
    email_pwd = ''.join(random.choice(letters) for i in range(5))
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/register")
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Jan")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("Novak")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd + "@gmail.com")
    context.driver.find_element(By.ID, "input-telephone").click()
    context.driver.find_element(By.ID, "input-telephone").send_keys("9999999999")
    context.driver.find_element(By.ID, "input-address-1").click()
    context.driver.find_element(By.ID, "input-address-1").send_keys("Hlavna 120")
    context.driver.find_element(By.ID, "input-city").click()
    context.driver.find_element(By.ID, "input-city").send_keys("Zilina")
    context.driver.find_element(By.ID, "input-postcode").click()
    context.driver.find_element(By.ID, "input-postcode").send_keys("12345")
    context.driver.find_element(By.ID, "input-country").click()
    dropdown = context.driver.find_element(By.ID, "input-country")
    dropdown.find_element(By.XPATH, "//option[. = 'Slovak Republic']").click()
    context.driver.find_element(By.CSS_SELECTOR, "option:nth-child(201)").click()
    context.driver.find_element(By.ID, "input-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    time.sleep(1)
    dropdown.find_element(By.XPATH, "//option[. = 'Žilinský']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-zone > option:nth-child(9)").click()
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd)
    context.driver.find_element(By.ID, "input-confirm").click()
    context.driver.find_element(By.ID, "input-confirm").send_keys(email_pwd)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    time.sleep(1)


# Test 01
@given('User is prompted to register')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/register")


@when('User fills required personal information')
def step_imp1(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Jan")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("Novak")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd + "@gmail.com")
    context.driver.find_element(By.ID, "input-telephone").click()
    context.driver.find_element(By.ID, "input-telephone").send_keys("9999999999")
    context.driver.find_element(By.ID, "input-address-1").click()
    context.driver.find_element(By.ID, "input-address-1").send_keys("Hlavna 120")
    context.driver.find_element(By.ID, "input-city").click()
    context.driver.find_element(By.ID, "input-city").send_keys("Zilina")
    context.driver.find_element(By.ID, "input-postcode").click()
    context.driver.find_element(By.ID, "input-postcode").send_keys("12345")
    context.driver.find_element(By.ID, "input-country").click()
    dropdown = context.driver.find_element(By.ID, "input-country")
    dropdown.find_element(By.XPATH, "//option[. = 'Slovak Republic']").click()
    context.driver.find_element(By.CSS_SELECTOR, "option:nth-child(201)").click()
    context.driver.find_element(By.ID, "input-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Žilinský']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-zone > option:nth-child(9)").click()
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd)
    context.driver.find_element(By.ID, "input-confirm").click()
    context.driver.find_element(By.ID, "input-confirm").send_keys(email_pwd)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    time.sleep(1)


@then('User\'s account has been created')
def step_imp1(context):
    assert ("Your Account Has Been Created!" in context.driver.page_source)
    context.driver.quit()


# Test 02
@given('Unregistered user is on login page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    context.driver.find_element(By.LINK_TEXT, "Login").click()
    context.driver.find_element(By.CSS_SELECTOR, ".list-group-item:nth-child(4)").click()


@when('Unregistered user clicks account specific tabs')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Address Book").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp2(context):
    context.driver.find_element(By.LINK_TEXT, "Wish List").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp3(context):
    context.driver.find_element(By.LINK_TEXT, "Order History").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp4(context):
    context.driver.find_element(By.LINK_TEXT, "Downloads").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp5(context):
    context.driver.find_element(By.LINK_TEXT, "Recurring payments").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp6(context):
    context.driver.find_element(By.LINK_TEXT, "Reward Points").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp7(context):
    context.driver.find_element(By.LINK_TEXT, "Returns").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp8(context):
    context.driver.find_element(By.LINK_TEXT, "Transactions").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


def step_imp9(context):
    context.driver.find_element(By.LINK_TEXT, "Newsletter").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)


@then('Unregistered user will not access these tabs')
def step_imp1(context):
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)
    context.driver.quit()


# Test 03
@given('User is on Account Information tab')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    reg(context)
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/edit")


@when('User changes information')
def step_imp1(context):
    time.sleep(1)
    context.driver.find_element(By.ID, "input-firstname").clear()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Janko")
    context.driver.find_element(By.ID, "input-lastname").clear()
    context.driver.find_element(By.ID, "input-lastname").send_keys("Novotny")
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then('Information are updated')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Edit your account information").click()
    assert (context.driver.find_element(By.ID, "input-firstname").get_attribute('value') == 'Janko')
    assert (context.driver.find_element(By.ID, "input-lastname").get_attribute('value') == 'Novotny')
    context.driver.quit()


# Test 04
@given('User\'s selected Contact Us tab')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=information/contact")


@when('User submits enquiry')
def step_imp1(context):
    context.driver.find_element(By.ID, "input-name").send_keys("Janko")
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd + "@gmail.com")
    context.driver.find_element(By.ID, "input-enquiry").send_keys(lorem_ipsum)
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then('User is prompted about successful submission')
def step_imp1(context):
    assert ("Your enquiry has been successfully sent to the store owner!" in context.driver.page_source)
    context.driver.quit()


# Test 05
@given('User has changed account password')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/register")
    context.driver.find_element(By.ID, "input-firstname").send_keys("Jan")
    context.driver.find_element(By.ID, "input-lastname").send_keys("Novak")
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd05 + "@gmail.com")
    context.driver.find_element(By.ID, "input-telephone").send_keys("9999999999")
    context.driver.find_element(By.ID, "input-address-1").send_keys("Hlavna 120")
    context.driver.find_element(By.ID, "input-city").send_keys("Zilina")
    context.driver.find_element(By.ID, "input-postcode").send_keys("12345")
    dropdown = context.driver.find_element(By.ID, "input-country")
    dropdown.find_element(By.XPATH, "//option[. = 'Slovak Republic']").click()
    context.driver.find_element(By.CSS_SELECTOR, "option:nth-child(201)").click()
    context.driver.find_element(By.ID, "input-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Žilinský']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-zone > option:nth-child(9)").click()
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd05)
    context.driver.find_element(By.ID, "input-confirm").click()
    context.driver.find_element(By.ID, "input-confirm").send_keys(email_pwd05)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    time.sleep(1)
    context.driver.find_element(By.LINK_TEXT, "Change your password").click()
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd05.upper())
    context.driver.find_element(By.ID, "input-confirm").send_keys(email_pwd05.upper())
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@when('User tries to log in')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Logout").click()
    context.driver.find_element(By.LINK_TEXT, "Login").click()
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd05 + "@gmail.com")
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd05.upper())
    context.driver.find_element(By.ID, "input-password").send_keys(Keys.ENTER)
    time.sleep(1)


@then('New password will work instead of old')
def step_imp1(context):
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/account" == context.driver.current_url)
    context.driver.quit()


# Test 06
@given('User is on Forgotten Password page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/forgotten")
    time.sleep(1)


@when('User fills valid email address and continues')
def step_imp1(context):
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd + "@gmail.com")
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then('User is informed that confirmation link has been sent')
def step_imp1(context):
    assert ("An email with a confirmation link has been sent your email address." in context.driver.page_source)
    context.driver.quit()


#  Test 07
# @given('User is on Forgotten Password page')

@when('User fills invalid email address and continues')
def step_imp1(context):
    context.driver.find_element(By.ID, "input-email").send_keys("INVALID@gmail.com")
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then('User is informed that operation failed and email was invalid')
def step_imp1(context):
    assert ("Warning: The E-Mail Address was not found in our records, please try again!" in context.driver.page_source)
    context.driver.quit()


#  Test 08
@given('User is on Address Book Entries page with address entries')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    context.driver.find_element(By.LINK_TEXT, "Address Book").click()
    context.driver.find_element(By.LINK_TEXT, "New Address").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Astolfo")
    context.driver.find_element(By.ID, "input-lastname").send_keys("of Charlemagne")
    context.driver.find_element(By.ID, "input-address-1").send_keys("Camelot 1/1")
    context.driver.find_element(By.ID, "input-city").send_keys("Camelot")
    context.driver.find_element(By.ID, "input-postcode").send_keys("54321")
    context.driver.find_element(By.ID, "input-country").click()
    dropdown = context.driver.find_element(By.ID, "input-country")
    dropdown.find_element(By.XPATH, "//option[. = 'United Kingdom']").click()
    context.driver.find_element(By.ID, "input-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Somerset']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-zone > option:nth-child(5)").click()


@when('User clicks Delete button')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.find_element(By.CSS_SELECTOR, ".table > tbody:nth-child(1) > tr:nth-child(2) > td:nth-child(2) > a:nth-child(2)").click()


@then('Address entry is not shown anymore')
def step_imp1(context):
    time.sleep(1)
    assert ("Astolfo" not in context.driver.page_source)
    assert ("of Charlemagne" not in context.driver.page_source)
    assert ("Camelot" not in context.driver.page_source)
    context.driver.quit()


#  Test 09
@given('User is on Address Book Entries page with defaulted address')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    reg(context)
    context.driver.find_element(By.LINK_TEXT, "Address Book").click()


@when('User clicks Delete button on defaulted address')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Delete").click()


@then('User is prompted that he cannot delete this address')
def step_imp1(context):
    assert ("Warning: You can not delete your default address!" in context.driver.page_source)
    context.driver.quit()


# Test 10
@given('User has affiliate account')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    reg(context)
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")
    context.driver.find_element(By.LINK_TEXT, "Affiliates").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Janko")
    context.driver.find_element(By.ID, "input-lastname").send_keys("Novotny")
    context.driver.find_element(By.ID, "input-email").send_keys(email_pwd + "@gmail.com")
    context.driver.find_element(By.ID, "input-telephone").send_keys("9999999999")
    context.driver.find_element(By.ID, "input-address-1").send_keys("Hlavna 120")
    context.driver.find_element(By.ID, "input-city").send_keys("Zilina")
    context.driver.find_element(By.ID, "input-postcode").send_keys("54321")
    context.driver.find_element(By.ID, "input-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Conwy']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-zone > option:nth-child(20)").click()
    context.driver.find_element(By.ID, "input-password").send_keys(email_pwd)
    context.driver.find_element(By.ID, "input-confirm").send_keys(email_pwd)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


@when('User chooses item for tracking')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Custom Affiliate Tracking Code").click()
    context.driver.find_element(By.ID, "input-generator").click()
    time.sleep(1)
    context.driver.find_element(By.LINK_TEXT, "Apple Cinema 30\"").click()


@then('Site generates link that directs user to item\'s page')
def step_imp1(context):
    assert (context.driver.find_element(By.ID, "input-code").get_attribute('value') in context.driver.find_element(By.ID, "input-link").get_attribute('value'))
    context.driver.quit()


#  Test 11
@given('User is on Newsletter Subscription page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    reg(context)
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/newsletter")


@when('User changes subscribe option and continues')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, ".radio-inline:nth-child(1)").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then('User receives feedback about subscription update')
def step_imp1(context):
    assert ("Success: Your newsletter subscription has been successfully updated!" in context.driver.page_source)
    context.driver.quit()


#  Test 12
@given('User is logged in and on My Account page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=account/login")
    reg(context)
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/account" == context.driver.current_url)


@when('User clicks Logout')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Logout").click()


@then('User is redirected from current page')
def step_imp1(context):
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/logout" == context.driver.current_url)


@then('User is prompted about logout change')
def step_imp1(context):
    assert ("You have been logged off your account." in context.driver.page_source)


@then('User may login with credentials now')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    context.driver.find_element(By.LINK_TEXT, "My Account").click()
    context.driver.find_element(By.LINK_TEXT, "Login").click()
    assert ("http://pat.fit.vutbr.cz:8074/index.php?route=account/login" == context.driver.current_url)
    context.driver.quit()


#  Test 13
@given('User has added items in Cart and is not logged in')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=product/category&path=24")
    context.driver.find_element(By.LINK_TEXT, "HTC Touch HD").click()
    context.driver.find_element(By.ID, "button-cart").click()
    context.driver.find_element(By.CSS_SELECTOR, ".breadcrumb > li:nth-child(2) > a").click()
    context.driver.find_element(By.LINK_TEXT, "iPhone").click()
    context.driver.find_element(By.ID, "button-cart").click()


@when('User logs in')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "My Account").click()
    reg(context)


@then('Items in cart are preserved')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, "li:nth-child(4) .hidden-xs").click()
    assert ("HTC Touch HD" in context.driver.page_source)
    assert ("iPhone" in context.driver.page_source)
    context.driver.quit()


#  Test 14
@given('User ordered item and is on Order Information page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    time.sleep(1)
    context.driver.find_element(By.LINK_TEXT, "Phones & PDAs").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(2) button:nth-child(1)").click()
    context.driver.find_element(By.CSS_SELECTOR, "li:nth-child(5) .hidden-xs").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-payment-address").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-shipping-address").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-shipping-method").click()
    time.sleep(1)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.ID, "button-payment-method").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-confirm").click()
    time.sleep(1)
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    context.driver.find_element(By.CSS_SELECTOR, ".caret").click()
    context.driver.find_element(By.CSS_SELECTOR, ".dropdown-menu > li:nth-child(1) > a").click()
    context.driver.find_element(By.LINK_TEXT, "Order History").click()


@when('User clicks Return button, fills information and submits')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .fa").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .btn-danger > .fa").click()
    context.driver.find_element(By.CSS_SELECTOR, ".radio:nth-child(3) > label").click()
    context.driver.find_element(By.ID, "input-comment").send_keys("covfefe")
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    time.sleep(1)


@then('Item will be shown in Product Returns page')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Returns").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .btn").click()
    time.sleep(1)
    assert ("iPhone" in context.driver.page_source)
    assert ("covfefe" in context.driver.page_source)
    context.driver.quit()


#  Test 15
@given('User is on item page')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    context.driver.find_element(By.LINK_TEXT, "Phones & PDAs").click()


@when('User clicks Add to Wish list')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(1) button:nth-child(2)").click()


@then('Item will be shown in Wish list')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, "#wishlist-total > .hidden-xs").click()
    assert ("HTC Touch HD" in context.driver.page_source)
    context.driver.find_element(By.CSS_SELECTOR, ".btn-danger:nth-child(2)").click()
    context.driver.quit()


#  Test 16
@given('User is on Wish List page with added item')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    context.driver.find_element(By.LINK_TEXT, "Phones & PDAs").click()
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(1) button:nth-child(2)").click()
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(1) .fa-heart").click()
    context.driver.find_element(By.CSS_SELECTOR, "#wishlist-total > .hidden-xs").click()


@when('User clicks Remove')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-danger:nth-child(2)").click()


@then('User is prompted about change')
def step_imp1(context):
    assert ("Success: You have modified your wish list!" in context.driver.page_source)


@then('Item is no longer shown in Wish List')
def step_imp1(context):
    assert ("HTC Touch HD" not in context.driver.page_source)
    context.driver.quit()


#  Test 17
@given('User is on Cart page with items')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    time.sleep(1)
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")
    context.driver.find_element(By.LINK_TEXT, "Cameras").click()
    context.driver.find_element(By.LINK_TEXT, "Nikon D300").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-cart").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, "a:nth-child(1) > strong").click()


@when('User exceeds item\'s stock quantity and updates cart')
def step_imp1(context):
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".table-responsive > table:nth-child(1) > tbody:nth-child(2) > tr:nth-child(1) > td:nth-child(4) > div:nth-child(1) > input:nth-child(1)").send_keys("199999")
    context.driver.find_element(By.CSS_SELECTOR, ".fa-refresh").click()


@then('User is prompted about items\'s inavailbility')
def step_imp1(context):
    assert ("Products marked with *** are not available in the desired quantity or not in stock!" in context.driver.page_source)
    context.driver.find_element(By.CSS_SELECTOR, ".btn-danger:nth-child(2)").click()
    context.driver.quit()


#  Test 18
@given('Logged user is on Checkout page while having item(s) in cart')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    reg(context)
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")
    context.driver.find_element(By.LINK_TEXT, "Cameras").click()
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(2) button:nth-child(1)").click()
    context.driver.find_element(By.CSS_SELECTOR, "li:nth-child(5) .hidden-xs").click()


@when('User proceeds through Checkout Steps and click on Confirm Order')
def step_imp1(context):
    time.sleep(1)
    context.driver.find_element(By.ID, "button-payment-address").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-shipping-address").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-shipping-method").click()
    time.sleep(1)
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.ID, "button-payment-method").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-confirm").click()
    time.sleep(1)


@then('User is prompted about success of operation')
def step_imp1(context):
    assert ("Your order has been placed!" in context.driver.page_source)
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    context.driver.find_element(By.CSS_SELECTOR, ".dropdown .hidden-xs").click()


@then('Order is shown on Order History page')
def step_imp1(context):
    context.driver.find_element(By.LINK_TEXT, "Order History").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .fa").click()
    assert ("Nikon D300" in context.driver.page_source)
    context.driver.quit()


#  Test 19
@given('User ordered unavailable item')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")
    context.driver.find_element(By.LINK_TEXT, "Tablets").click()
    context.driver.find_element(By.CSS_SELECTOR, ".button-group > button:nth-child(1)").click()


@when('User clicks checkout')
def step_imp1(context):
    context.driver.find_element(By.CSS_SELECTOR, "li:nth-child(5) .hidden-xs").click()
    time.sleep(1)


@then('User is prompted about item\'s unavailability')
def step_imp1(context):
    assert ("Products marked with *** are not available in the desired quantity or not in stock!" in context.driver.page_source)


@then('Checkout is not initiated')
def step_imp1(context):
    assert ("Step 1: Checkout Options" not in context.driver.page_source)
    context.driver.quit()


#  Test 20
@given('User has empty Shopping cart')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")

#@when('User clicks Checkout')


@then('User is prompted about empty cart')
def step_imp1(context):
    assert ("Your shopping cart is empty!" in context.driver.page_source)

#@then('Checkout is not initiated')


#  Test 21
@given('User is on the last checkout step')
def step_imp1(context):
    context.driver = webdriver.Firefox()
    context.driver.get("http://pat.fit.vutbr.cz:8074/index.php?route=common/home")
    context.driver.find_element(By.LINK_TEXT, "Phones & PDAs").click()
    context.driver.find_element(By.CSS_SELECTOR, ".product-layout:nth-child(2) button:nth-child(1)").click()
    context.driver.find_element(By.LINK_TEXT, "Checkout").click()
    time.sleep(5)
    context.driver.find_element(By.CSS_SELECTOR, ".radio:nth-child(4) input").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-account").click()
    time.sleep(2)
    context.driver.find_element(By.ID, "input-payment-firstname").send_keys("TEST")
    context.driver.find_element(By.ID, "input-payment-lastname").send_keys("test")
    context.driver.find_element(By.ID, "input-payment-email").send_keys("test@gmail.com")
    context.driver.find_element(By.ID, "input-payment-telephone").send_keys("9999999999")
    context.driver.find_element(By.ID, "input-payment-address-1").send_keys("test")
    context.driver.find_element(By.ID, "input-payment-city").send_keys("test")
    context.driver.find_element(By.ID, "input-payment-postcode").send_keys("12345")
    context.driver.find_element(By.ID, "input-payment-zone").click()
    dropdown = context.driver.find_element(By.ID, "input-payment-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Aberdeen']").click()
    context.driver.find_element(By.CSS_SELECTOR, "#input-payment-zone > option:nth-child(2)").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-guest").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-shipping-method").click()
    time.sleep(1)
    context.driver.find_element(By.NAME, "agree").click()
    time.sleep(1)
    context.driver.find_element(By.ID, "button-payment-method").click()


@when('User removes items from cart')
def step_imp1(context):
    time.sleep(1)
    context.driver.find_element(By.ID, "cart-total").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".fa-times").click()


@then('Checkout is cancelled')
def step_imp1(context):
    assert ("Your shopping cart is empty!" in context.driver.page_source)
    context.driver.quit()

