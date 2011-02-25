<?php

/**
 * @file
 * Defines an extended SOAP client for use with CyberSource.
 */


/**
 * Adds necessary header information to the CyberSource SOAP API request.
 */
class CommerceCyberSourceSoapClient extends SoapClient {
	public $user = '';
	public $password = '';

  function __construct($wsdl, $options = NULL, $user, $password) {
	  $this->user = $user;
		$this->password = $password;
    parent::__construct($wsdl, $options);
  }

  // This section inserts the UsernameToken information in the SOAP message.
  function __doRequest($request, $location, $action, $version) {
    $soapHeader = '<SOAP-ENV:Header xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:wsse="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><wsse:Security SOAP-ENV:mustUnderstand="1"><wsse:UsernameToken><wsse:Username>' . strtolower($this->user) . '</wsse:Username><wsse:Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText">' . $this->password . '</wsse:Password></wsse:UsernameToken></wsse:Security></SOAP-ENV:Header>';

    $requestDOM = new DOMDocument('1.0');
    $soapHeaderDOM = new DOMDocument('1.0');

    try {
      $requestDOM->loadXML($request);
	    $soapHeaderDOM->loadXML($soapHeader);

	    $node = $requestDOM->importNode($soapHeaderDOM->firstChild, TRUE);
	    $requestDOM->firstChild->insertBefore($node, $requestDOM->firstChild->firstChild);

      $request = $requestDOM->saveXML();
		}
		catch (DOMException $e) {
      die(t('Error adding UsernameToken: @code', array('@code' => $e->code)));
    }

    return parent::__doRequest($request, $location, $action, $version);
  }
}
