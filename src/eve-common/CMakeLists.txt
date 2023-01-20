#
# CMake build system file for EVEmu.
#
# Author: Bloody.Rabbit
#

##############
# Initialize #
##############
SET( TARGET_NAME        "eve-common" )
SET( TARGET_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/${TARGET_NAME}" )
SET( TARGET_SOURCE_DIR  "${PROJECT_SOURCE_DIR}/src/${TARGET_NAME}" )

#########
# Files #
#########
SET( INCLUDE
     "${TARGET_INCLUDE_DIR}/eve-common.h"
     "${TARGET_INCLUDE_DIR}/EVE_Consts.h"
     "${TARGET_INCLUDE_DIR}/EVE_Corp.h"
     "${TARGET_INCLUDE_DIR}/EVE_Defines.h"
     "${TARGET_INCLUDE_DIR}/EVE_Dungeon.h"
     "${TARGET_INCLUDE_DIR}/EVE_Effects.h"
     "${TARGET_INCLUDE_DIR}/EVE_Flags.h"
     "${TARGET_INCLUDE_DIR}/EVE_Inventory.h"
     "${TARGET_INCLUDE_DIR}/EVE_LSC.h"
     "${TARGET_INCLUDE_DIR}/EVE_Mail.h"
     "${TARGET_INCLUDE_DIR}/EVE_Market.h"
     "${TARGET_INCLUDE_DIR}/EVE_Planet.h"
     "${TARGET_INCLUDE_DIR}/EVE_POS.h"
     "${TARGET_INCLUDE_DIR}/EVE_Roles.h"
     "${TARGET_INCLUDE_DIR}/EVE_Scanning.h"
     "${TARGET_INCLUDE_DIR}/EVE_Skills.h"
     "${TARGET_INCLUDE_DIR}/EVE_Spawn.h"
     "${TARGET_INCLUDE_DIR}/EVE_Typedefs.h"
     "${TARGET_INCLUDE_DIR}/EVE_Wallet.h"
     "${TARGET_INCLUDE_DIR}/EVE_Station.h")
SET( SOURCE
     "" )

SET( auth_INCLUDE
     "${TARGET_INCLUDE_DIR}/auth/BinAsciiModule.h"
     "${TARGET_INCLUDE_DIR}/auth/PasswordModule.h"
     "${TARGET_INCLUDE_DIR}/auth/ShaModule.h" )
SET( auth_SOURCE
     "${TARGET_SOURCE_DIR}/auth/PasswordModule.cpp" )

SET( cache_INCLUDE
     "${TARGET_INCLUDE_DIR}/cache/CachedObjectMgr.h" )
SET( cache_SOURCE
     "${TARGET_SOURCE_DIR}/cache/CachedObjectMgr.cpp" )

SET( database_INCLUDE
     "${TARGET_INCLUDE_DIR}/database/EVEDBUtils.h" )
SET( database_SOURCE
     "${TARGET_SOURCE_DIR}/database/EVEDBUtils.cpp" )

SET( destiny_INCLUDE
     "${TARGET_INCLUDE_DIR}/destiny/DestinyBinDump.h"
     "${TARGET_INCLUDE_DIR}/destiny/DestinyStructs.h" )
SET( destiny_SOURCE
     "${TARGET_SOURCE_DIR}/destiny/DestinyBinDump.cpp" )

SET( marshal_INCLUDE
     "${TARGET_INCLUDE_DIR}/marshal/EVEMarshal.h"
     "${TARGET_INCLUDE_DIR}/marshal/EVEMarshalOpcodes.h"
     "${TARGET_INCLUDE_DIR}/marshal/EVEMarshalStringTable.h"
     "${TARGET_INCLUDE_DIR}/marshal/EVEUnmarshal.h" )
SET( marshal_SOURCE
     "${TARGET_SOURCE_DIR}/marshal/EVEMarshal.cpp"
     "${TARGET_SOURCE_DIR}/marshal/EVEMarshalStringTable.cpp"
     "${TARGET_SOURCE_DIR}/marshal/EVEUnmarshal.cpp" )

SET( network_INCLUDE
     "${TARGET_INCLUDE_DIR}/network/EVEPktDispatch.h"
     "${TARGET_INCLUDE_DIR}/network/python/EVEPacketAddress.h"
     "${TARGET_INCLUDE_DIR}/network/python/EVEPacket.h"
     "${TARGET_INCLUDE_DIR}/network/python/EVECallStream.h"
     "${TARGET_INCLUDE_DIR}/network/python/EVENotificationStream.h"
     "${TARGET_INCLUDE_DIR}/network/python/EVEPasswordString.h"
     "${TARGET_INCLUDE_DIR}/network/EVESession.h"
     "${TARGET_INCLUDE_DIR}/network/EVETCPConnection.h"
     "${TARGET_INCLUDE_DIR}/network/EVETCPServer.h"
     "${TARGET_INCLUDE_DIR}/network/packet_types.h" )
SET( network_SOURCE
     "${TARGET_SOURCE_DIR}/network/EVEPktDispatch.cpp"
     "${TARGET_SOURCE_DIR}/network/python/EVEPacketAddress.cpp"
     "${TARGET_SOURCE_DIR}/network/python/EVEPacket.cpp"
     "${TARGET_SOURCE_DIR}/network/python/EVECallStream.cpp"
     "${TARGET_SOURCE_DIR}/network/python/EVENotificationStream.cpp"
     "${TARGET_SOURCE_DIR}/network/python/EVEPasswordString.cpp"
     "${TARGET_SOURCE_DIR}/network/EVESession.cpp"
     "${TARGET_SOURCE_DIR}/network/EVETCPConnection.cpp" )

SET( python_INCLUDE
     "${TARGET_INCLUDE_DIR}/python/PyDataType.h"
     "${TARGET_INCLUDE_DIR}/python/PyNone.h"
     "${TARGET_INCLUDE_DIR}/python/PyInt.h"
     "${TARGET_INCLUDE_DIR}/python/PyFloat.h"
     "${TARGET_INCLUDE_DIR}/python/PyString.h"
     "${TARGET_INCLUDE_DIR}/python/PyToken.h"
     "${TARGET_INCLUDE_DIR}/python/PyBuffer.h"
     "${TARGET_INCLUDE_DIR}/python/PyBool.h"
     "${TARGET_INCLUDE_DIR}/python/PyTuple.h"
     "${TARGET_INCLUDE_DIR}/python/PyList.h"
     "${TARGET_INCLUDE_DIR}/python/PyDict.h"
     "${TARGET_INCLUDE_DIR}/python/PyObject.h"
     "${TARGET_INCLUDE_DIR}/python/PyObjectEx.h"
     "${TARGET_INCLUDE_DIR}/python/PyPackedRow.h"
     "${TARGET_INCLUDE_DIR}/python/PySubStruct.h"
     "${TARGET_INCLUDE_DIR}/python/PySubStream.h"
     "${TARGET_INCLUDE_DIR}/python/PyVisitor.h"
     "${TARGET_INCLUDE_DIR}/python/PyPfxVisitor.h"
     "${TARGET_INCLUDE_DIR}/python/PyDumpVisitor.h"
     "${TARGET_INCLUDE_DIR}/python/PyLogDumpVisitor.h"
     "${TARGET_INCLUDE_DIR}/python/PyFileDumpVisitor.h"
     "${TARGET_INCLUDE_DIR}/python/exceptions/CustomError.h"
     "${TARGET_INCLUDE_DIR}/python/exceptions/UserError.h"
     "${TARGET_INCLUDE_DIR}/python/exceptions/GPSTransportClosed.h"
     "${TARGET_INCLUDE_DIR}/python/exceptions/PyException.h"
     "${TARGET_INCLUDE_DIR}/python/database/DBRowDescriptor.h"
     "${TARGET_INCLUDE_DIR}/python/database/CRowset.h"
     "${TARGET_INCLUDE_DIR}/python/database/CIndexedRowset.h"
     "${TARGET_INCLUDE_DIR}/python/database/CFilterRowset.h"
     "${TARGET_INCLUDE_DIR}/python/utils/VectorWrapper.h"
     "${TARGET_INCLUDE_DIR}/python/utils/UnorderedMapWrapper.h")
SET( python_SOURCE
     "${TARGET_SOURCE_DIR}/python/PyDataType.cpp"
     "${TARGET_SOURCE_DIR}/python/PyNone.cpp"
     "${TARGET_SOURCE_DIR}/python/PyInt.cpp"
     "${TARGET_SOURCE_DIR}/python/PyFloat.cpp"
     "${TARGET_SOURCE_DIR}/python/PyString.cpp"
     "${TARGET_SOURCE_DIR}/python/PyToken.cpp"
     "${TARGET_SOURCE_DIR}/python/PyBuffer.cpp"
     "${TARGET_SOURCE_DIR}/python/PyBool.cpp"
     "${TARGET_SOURCE_DIR}/python/PyTuple.cpp"
     "${TARGET_SOURCE_DIR}/python/PyList.cpp"
     "${TARGET_SOURCE_DIR}/python/PyDict.cpp"
     "${TARGET_SOURCE_DIR}/python/PyObject.cpp"
     "${TARGET_SOURCE_DIR}/python/PyObjectEx.cpp"
     "${TARGET_SOURCE_DIR}/python/PyPackedRow.cpp"
     "${TARGET_SOURCE_DIR}/python/PySubStruct.cpp"
     "${TARGET_SOURCE_DIR}/python/PySubStream.cpp"
     "${TARGET_SOURCE_DIR}/python/PyVisitor.cpp"
     "${TARGET_SOURCE_DIR}/python/PyPfxVisitor.cpp"
     "${TARGET_SOURCE_DIR}/python/PyDumpVisitor.cpp"
     "${TARGET_SOURCE_DIR}/python/PyLogDumpVisitor.cpp"
     "${TARGET_SOURCE_DIR}/python/PyFileDumpVisitor.cpp"
     "${TARGET_SOURCE_DIR}/python/exceptions/CustomError.cpp"
     "${TARGET_SOURCE_DIR}/python/exceptions/UserError.cpp"
     "${TARGET_SOURCE_DIR}/python/exceptions/GPSTransportClosed.cpp"
     "${TARGET_SOURCE_DIR}/python/exceptions/PyException.cpp"
     "${TARGET_SOURCE_DIR}/python/database/DBRowDescriptor.cpp"
     "${TARGET_SOURCE_DIR}/python/database/CRowset.cpp"
     "${TARGET_SOURCE_DIR}/python/database/CIndexedRowset.cpp"
     "${TARGET_SOURCE_DIR}/python/database/CFilterRowset.cpp"
     "${TARGET_SOURCE_DIR}/python/utils/VectorWrapper.cpp"
     "${TARGET_SOURCE_DIR}/python/utils/UnorderedMapWrapper.cpp")

SET( tables_INCLUDE
     "${TARGET_INCLUDE_DIR}/tables/invCategories.h"
     "${TARGET_INCLUDE_DIR}/tables/invGroups.h"
     "${TARGET_INCLUDE_DIR}/tables/invTypes.h" )
SET( tables_SOURCE
     "" )

SET( utils_INCLUDE
     "${TARGET_INCLUDE_DIR}/utils/EvEMath.h"
     "${TARGET_INCLUDE_DIR}/utils/EVEUtils.h"
     "${TARGET_INCLUDE_DIR}/utils/EvilNumber.h")
SET( utils_SOURCE
     "${TARGET_SOURCE_DIR}/utils/EvEMath.cpp"
     "${TARGET_SOURCE_DIR}/utils/EVEUtils.cpp"
     "${TARGET_SOURCE_DIR}/utils/EvilNumber.cpp")

#####################
# Setup the library #
#####################
SOURCE_GROUP( "src"                  FILES ${INCLUDE} )
SOURCE_GROUP( "src\\auth"            FILES ${auth_INCLUDE} )
SOURCE_GROUP( "src\\cache"           FILES ${cache_INCLUDE} )
SOURCE_GROUP( "src\\database"        FILES ${database_INCLUDE} )
SOURCE_GROUP( "src\\destiny"         FILES ${destiny_INCLUDE} )
SOURCE_GROUP( "src\\marshal"         FILES ${marshal_INCLUDE} )
SOURCE_GROUP( "src\\network"         FILES ${network_INCLUDE} )
SOURCE_GROUP( "src\\python"          FILES ${python_INCLUDE} )
SOURCE_GROUP( "src\\tables"          FILES ${tables_INCLUDE} )
SOURCE_GROUP( "src\\utils"           FILES ${utils_INCLUDE} )

SOURCE_GROUP( "src"                  FILES ${SOURCE} )
SOURCE_GROUP( "src\\auth"            FILES ${auth_SOURCE} )
SOURCE_GROUP( "src\\cache"           FILES ${cache_SOURCE} )
SOURCE_GROUP( "src\\database"        FILES ${database_SOURCE} )
SOURCE_GROUP( "src\\destiny"         FILES ${destiny_SOURCE} )
SOURCE_GROUP( "src\\marshal"         FILES ${marshal_SOURCE} )
SOURCE_GROUP( "src\\network"         FILES ${network_SOURCE} )
SOURCE_GROUP( "src\\python"          FILES ${python_SOURCE} )
SOURCE_GROUP( "src\\tables"          FILES ${tables_SOURCE} )
SOURCE_GROUP( "src\\utils"           FILES ${utils_SOURCE} )

ADD_LIBRARY( "${TARGET_NAME}"
             ${INCLUDE}                ${SOURCE}
             ${auth_INCLUDE}           ${auth_SOURCE}
             ${cache_INCLUDE}          ${cache_SOURCE}
             ${database_INCLUDE}       ${database_SOURCE}
             ${destiny_INCLUDE}        ${destiny_SOURCE}
             ${marshal_INCLUDE}        ${marshal_SOURCE}
             ${network_INCLUDE}        ${network_SOURCE}
             ${python_INCLUDE}         ${python_SOURCE}
             ${python_classes_INCLUDE} ${python_classes_SOURCE}
             ${tables_INCLUDE}         ${tables_SOURCE}
             ${utils_INCLUDE}          ${utils_SOURCE} )


target_precompile_headers( "${TARGET_NAME}" PUBLIC
                  "${TARGET_INCLUDE_DIR}/eve-common.h" )
TARGET_INCLUDE_DIRECTORIES( "${TARGET_NAME}"
                            ${UTF8CPP_INCLUDE_DIRS}
                            ${UTILS_INCLUDE_DIRS}
                            ${eve-core_INCLUDE_DIRS}
                            "${TARGET_INCLUDE_DIR}" )
TARGET_LINK_LIBRARIES( "${TARGET_NAME}"
                       ${UTILS_LIBRARIES}
                       "eve-core" )
