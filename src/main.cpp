
/**
 * SYNTAX:
 *
 * String Firestore::Documents::createDocument(<AsyncClient>, <Firestore::Parent>, <documentPath>, <DocumentMask>, <Document>);
 *
 * String Firestore::Documents::createDocument(<AsyncClient>, <Firestore::Parent>, <collectionId>, <documentId>, <DocumentMask>, <Document>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <documentPath> - The relative path of document to create in the collection.
 * <DocumentMask> - The fields to return. If not set, returns all fields. Use comma (,) to separate between the field names.
 * <collectionId> - The document id of document to be created.
 * <documentId> - The relative path of document collection id to create the document.
 * <Document> - The Firestore document.
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

// wifi client
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseClient.h>

// iclude date.cpp
#include "date.h"

// Your WiFi credentials.
#define WIFI_SSID "xeph★s"
#define WIFI_PASSWORD "xephas096z"

//   SSID:	xeph★s
// Protocol:	Wi-Fi 4 (802.11n)
// Security type:	WPA2-Personal
// Manufacturer:	Intel Corporation
// Description:	Intel(R) Dual Band Wireless-AC 8265
// Driver version:	20.70.30.1
// Network band:	2.4 GHz
// Network channel:	9
// Link speed (Receive/Transmit):	72/72 (Mbps)
// Link-local IPv6 address:	fe80::6703:d220:755d:9d7b%21
// IPv4 address:	192.168.155.63
// IPv4 DNS servers:	192.168.155.22 (Unencrypted)
// Physical address (MAC):	28-C6-3F-60-BF-F3


// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "AIzaSyAH-JWnXtIktk7dHVTOYXR8c6weZtYirps"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "test@wcmcs.com"
#define USER_PASSWORD "Test@WCMCS096"
// #define DATABASE_URL "URL"

// Define the project ID
#define FIREBASE_PROJECT_ID "mukfpl"

// define the functions
//  and variables
void authHandler();

void printResult(AsyncResult &aResult);
void printError(int code, const String &msg);

String getTimestampString(uint64_t sec, uint32_t nano);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Firestore::Documents Docs;

AsyncResult aResult_no_callback;

bool taskCompleted = false;

// sections db path
String sectionsCollection = "sections";

// current section e.g kitchen, bathroom, living room etc
String currentSection = "kitchen";

// a function to get todays date and format it as
// todays date in format dd-mm-yyyy eg. 27-05-2024
String todaysDate = getTodayFormattedString();

int count = 0;

void setup()
{
    // * Wifi SetUp
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // * Firebase SetUp

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

    ssl_client.setInsecure();
    ssl_client.setBufferSizes(4096, 1024);

    initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);

    authHandler();

    app.getApp<Firestore::Documents>(Docs);

    // In case setting the external async result to the sync task (optional)
    // To unset, use unsetAsyncResult().
    aClient.setAsyncResult(aResult_no_callback);
}



void loop()
{
    authHandler();

    Docs.loop();

    if (app.ready() && !taskCompleted)
    {
        taskCompleted = true;

        // Note: If new document created under non-existent ancestor documents, that document will not appear in queries and snapshot
        // https://cloud.google.com/firestore/docs/using-console#non-existent_ancestor_documents.

        // We will create the document in the parent path "sectionsCollection/currentSection"
        // "sectionsCollection" is the collection id, "currentSection" is the document id in collection.
        String documentPath = sectionsCollection + "/" + currentSection + "/" + todaysDate + "/flow" + String(::count);

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        // double to hold the water flow rate
        Values::DoubleValue dblV(11000 / 1000.0f);

        // timestamp
        Values::TimestampValue tsV(getTimestampString(1712674441, 999999999));

        // string
        Values::StringValue strV("hello");

        // array
        // Values::ArrayValue arrV(Values::StringValue("test"));
        // arrV.add(Values::IntegerValue(20)).add(Values::BooleanValue(true));

        // // map
        // Values::MapValue mapV("name", Values::StringValue("wrench"));
        // mapV.add("mass", Values::StringValue("1.3kg")).add("count", Values::IntegerValue(3));

        // lat long
        // Values::GeoPointValue geoV(1.486284, 23.678198);

        Document<Values::Value> doc("amount", Values::Value(dblV));
        doc.add("timestamp", Values::Value(tsV)).add("myString", Values::Value(strV));

        // doc.add("myRef", Values::Value(refV))  .add("myBytes", Values::Value(bytesV));
        // doc.add("myBool", Values::Value(bolV)).add("myInt", Values::Value(intV)).add("myNull", Values::Value(nullV));
        // doc.add("myString", Values::Value(strV)).add("myArr", Values::Value(arrV)).add("myMap", Values::Value(mapV));
        // doc.add("myGeo", Values::Value(geoV));

        // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.
        ::count++;
        // taskCompleted = false;

        Serial.println("Create document... ");

        String payload = Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc);

        if (aClient.lastError().code() == 0)
            Serial.println(payload);
        else
            printError(aClient.lastError().code(), aClient.lastError().message());
    }
}



void authHandler()
{
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
    {
        // This JWT token process required for ServiceAuth and CustomAuth authentications
        JWT.loop(app.getAuth());
        printResult(aResult_no_callback);
    }
}

void printResult(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

String getTimestampString(uint64_t sec, uint32_t nano)
{
    if (sec > 0x3afff4417f)
        sec = 0x3afff4417f;

    if (nano > 0x3b9ac9ff)
        nano = 0x3b9ac9ff;

    time_t now;
    struct tm ts;
    char buf[80];
    now = sec;
    ts = *localtime(&now);

    String format = "%Y-%m-%dT%H:%M:%S";

    if (nano > 0)
    {
        String fraction = String(double(nano) / 1000000000.0f, 9);
        fraction.remove(0, 1);
        format += fraction;
    }
    format += "Z";

    strftime(buf, sizeof(buf), format.c_str(), &ts);
    return buf;
}

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}



