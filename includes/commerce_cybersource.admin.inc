<?php

/**
 * @file
 * Administrative forms for the CyberSource module.
 */


/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_cybersource_soap_capture_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $balance = commerce_payment_order_balance($order);

  if ($balance['amount'] > 0 && $balance['amount'] < $transaction->amount) {
    $default_amount = $balance['amount'];
  }
  else {
    $default_amount = $transaction->amount;
  }

  // Convert the price amount to a user friendly decimal value.
  $default_amount = commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code);

  $description = implode('<br />', array(
    t('Authorization: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
    t('Order balance: @balance', array('@balance' => commerce_currency_format($balance['amount'], $balance['currency_code']))),
  ));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Capture amount'),
    '#description' => $description,
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to capture?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Capture'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit handler: process a prior authorization capture via SOAP.
 */
function commerce_cybersource_soap_capture_form_submit($form, &$form_state) {
  $order = $form_state['order'];
  $transaction = $form_state['transaction'];
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);

  if ($response = commerce_cybersource_soap_capture($order, $transaction, $amount)) {
    // If the payment failed, display an error and rebuild the form.
    if ($response->ccCaptureReply->reasonCode != 100) {
      drupal_set_message(t('The capture failed with the following error: @reason', array('@reason' => commerce_cybersource_reason_message($response->ccCaptureReply->reasonCode))), 'error');
    }
    else {
      drupal_set_message(t('The payment was captured successfully.'));
      $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
    }
  }
  else {
    drupal_set_message(t('An error occurred communicate with CyberSource, and the payment was not captured.'), 'error');
  }
}
