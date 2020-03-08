# Library architecture

* `ZigbeeNetworkManager(ZigbeeNetworkInterfaceType)`: This is the main entry point for using the library. Here you can specify which interface should be used.
  * `ZigbeeNetworkInterface` Create a generic interface depending on the selected `ZigbeeNetworkInterfaceType`
    * `ZigbeeUartInterface`
        * `ZigbeeUartInterfaceNxp` (WIP)
        * `ZigbeeUartInterfaceDeconz` (TODO)
        * `ZigbeeUartInterfaceNordic` (TODO)

  * `ZigbeeNetwork(ZigbeeNetworkInterface)`
    PROPERTY `panId`
    PROPERTY `channel`
    PROPERTY `type`
    ...  

    * List of `ZigbeeNode(ZigbeeNetworkInterface)`
      PROPERTY `ieeeAddress`
      PROPERTY `shortAddress`
      PROPERTY `nodeDescriptor`
      PROPERTY `powerDescriptor`

      * List of `ZigbeeNodeEndpoint(ZigbeeNetworkInterface)`
        property `profile`
        property `deviceType`
        * List of `InputCluster`
            * List of `Attributes`
            * List of `Commands`
        * List of `OutputCluster`
            * List of `Attributes`
            * List of `Commands`

