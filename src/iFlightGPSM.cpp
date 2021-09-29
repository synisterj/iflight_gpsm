#include "iFlightGPSLib.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define _GPRMCterm   "GPRMC"
#define _GPGGAterm   "GPGGA"
#define _GNRMCterm   "GNRMC"
#define _GNGGAterm   "GNGGA"

iFlightGPSLib::iFlightGPSLib()
    : parity(0)
    , isChecksumTerm(false)
    , curSentenceType(GPS_SENTENCE_OTHER)
    , curTermNumber(0)
    , curTermOffset(0)
    , sentenceHasFix(false)
    , customElts(0)
    , customCandidates(0)
    , encodedCharCount(0)
    , sentencesWithFixCount(0)
    , failedChecksumCount(0)
    , passedChecksumCount(0)
{
    term[0] = '\0';
}

bool iFlightGPSLib::encode(char c)
{
    ++encodedCharCount;

    switch (c)
    {
    case ',':
        parity ^= (uint8_t)c;
    case '\r':
    case '\n':
    case '*':
    {
        bool isValidSentence = false;
        if (curTermOffset < sizeof(term))
        {
            term[curTermOffset] = 0;
            isValidSentence = endOfTermHandler();
        }
        ++curTermNumber;
        curTermOffset = 0;
        isChecksumTerm = c == '*';
        return isValidSentence;
    }
    break;

    case '$':
        curTermNumber = curTermOffset = 0;
        parity = 0;
        curSentenceType = GPS_SENTENCE_OTHER;
        isChecksumTerm = false;
        sentenceHasFix = false;
        return false;

    default:
        if (curTermOffset < sizeof(term) - 1)
            term[curTermOffset++] = c;
        if (!isChecksumTerm)
            parity ^= c;
        return false;
    }

    return false;
}

int iFlightGPSLib::fromHex(char a)
{
    if (a >= 'A' && a <= 'F')
        return a - 'A' + 10;
    else if (a >= 'a' && a <= 'f')
        return a - 'a' + 10;
    else
        return a - '0';
}

int32_t iFlightGPSLib::parseDecimal(const char* term)
{
    bool negative = *term == '-';
    if (negative) ++term;
    int32_t ret = 100 * (int32_t)atol(term);
    while (isdigit(*term)) ++term;
    if (*term == '.' && isdigit(term[1]))
    {
        ret += 10 * (term[1] - '0');
        if (isdigit(term[2]))
            ret += term[2] - '0';
    }
    return negative ? -ret : ret;
}

void iFlightGPSLib::parseDegrees(const char* term, RawDegrees& deg)
{
    uint32_t leftOfDecimal = (uint32_t)atol(term);
    uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
    uint32_t multiplier = 10000000UL;
    uint32_t tenMillionthsOfMinutes = minutes * multiplier;

    deg.deg = (int16_t)(leftOfDecimal / 100);

    while (isdigit(*term))
        ++term;

    if (*term == '.')
        while (isdigit(*++term))
        {
            multiplier /= 10;
            tenMillionthsOfMinutes += (*term - '0') * multiplier;
        }

    deg.billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
    deg.negative = false;
}

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

bool iFlightGPSLib::endOfTermHandler()
{
    if (isChecksumTerm)
    {
        byte checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
        if (checksum == parity)
        {
            passedChecksumCount++;
            if (sentenceHasFix)
                ++sentencesWithFixCount;

            switch (curSentenceType)
            {
            case GPS_SENTENCE_GPRMC:
                date.commit();
                time.commit();
                if (sentenceHasFix)
                {
                    location.commit();
                    speed.commit();
                    course.commit();
                }
                break;
            case GPS_SENTENCE_GPGGA:
                time.commit();
                if (sentenceHasFix)
                {
                    location.commit();
                    altitude.commit();
                }
                satellites.commit();
                hdop.commit();
                break;
            }
            for (iFlightGPSLibCustom* p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0; p = p->next)
                p->commit();
            return true;
        }

        else
        {
            ++failedChecksumCount;
        }

        return false;
    }
    if (curTermNumber == 0)
    {
        if (!strcmp(term, _GPRMCterm) || !strcmp(term, _GNRMCterm))
            curSentenceType = GPS_SENTENCE_GPRMC;
        else if (!strcmp(term, _GPGGAterm) || !strcmp(term, _GNGGAterm))
            curSentenceType = GPS_SENTENCE_GPGGA;
        else
            curSentenceType = GPS_SENTENCE_OTHER;
        for (customCandidates = customElts; customCandidates != NULL && strcmp(customCandidates->sentenceName, term) < 0; customCandidates = customCandidates->next);
        if (customCandidates != NULL && strcmp(customCandidates->sentenceName, term) > 0)
            customCandidates = NULL;

        return false;
    }

    if (curSentenceType != GPS_SENTENCE_OTHER && term[0])
        switch (COMBINE(curSentenceType, curTermNumber))
        {
        case COMBINE(GPS_SENTENCE_GPRMC, 1): // Time in both sentences
        case COMBINE(GPS_SENTENCE_GPGGA, 1):
            time.setTime(term);
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 2): // GPRMC validity
            sentenceHasFix = term[0] == 'A';
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 3): // Latitude
        case COMBINE(GPS_SENTENCE_GPGGA, 2):
            location.setLatitude(term);
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 4): // N/S
        case COMBINE(GPS_SENTENCE_GPGGA, 3):
            location.rawNewLatData.negative = term[0] == 'S';
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 5): // Longitude
        case COMBINE(GPS_SENTENCE_GPGGA, 4):
            location.setLongitude(term);
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 6): // E/W
        case COMBINE(GPS_SENTENCE_GPGGA, 5):
            location.rawNewLngData.negative = term[0] == 'W';
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 7): // Speed (GPRMC)
            speed.set(term);
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 8): // Course (GPRMC)
            course.set(term);
            break;
        case COMBINE(GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
            date.setDate(term);
            break;
        case COMBINE(GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
            sentenceHasFix = term[0] > '0';
            break;
        case COMBINE(GPS_SENTENCE_GPGGA, 7): // Satellites used (GPGGA)
            satellites.set(term);
            break;
        case COMBINE(GPS_SENTENCE_GPGGA, 8): // HDOP
            hdop.set(term);
            break;
        case COMBINE(GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
            altitude.set(term);
            break;
        }
    for (iFlightGPSLibCustom* p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0 && p->termNumber <= curTermNumber; p = p->next)
        if (p->termNumber == curTermNumber)
            p->set(term);

    return false;
}

/* static */
double iFlightGPSLib::distanceBetween(double lat1, double long1, double lat2, double long2)
{
    double delta = radians(long1 - long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = sq(delta);
    delta += sq(clat2 * sdlong);
    delta = sqrt(delta);
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);
    return delta * 6372795;
}

double iFlightGPSLib::courseTo(double lat1, double long1, double lat2, double long2)
{
    double dlon = radians(long2 - long1);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double a1 = sin(dlon) * cos(lat2);
    double a2 = sin(lat1) * cos(lat2) * cos(dlon);
    a2 = cos(lat1) * sin(lat2) - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0)
    {
        a2 += TWO_PI;
    }
    return degrees(a2);
}

const char* iFlightGPSLib::cardinal(double course)
{
    static const char* directions[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };
    int direction = (int)((course + 11.25f) / 22.5f);
    return directions[direction % 16];
}

void iFlightGPSLocation::commit()
{
    rawLatData = rawNewLatData;
    rawLngData = rawNewLngData;
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSLocation::setLatitude(const char* term)
{
    iFlightGPSLib::parseDegrees(term, rawNewLatData);
}

void iFlightGPSLocation::setLongitude(const char* term)
{
    iFlightGPSLib::parseDegrees(term, rawNewLngData);
}

double iFlightGPSLocation::lat()
{
    updated = false;
    double ret = rawLatData.deg + rawLatData.billionths / 1000000000.0;
    return rawLatData.negative ? -ret : ret;
}

double iFlightGPSLocation::lng()
{
    updated = false;
    double ret = rawLngData.deg + rawLngData.billionths / 1000000000.0;
    return rawLngData.negative ? -ret : ret;
}

void iFlightGPSDate::commit()
{
    date = newDate;
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSTime::commit()
{
    time = newTime;
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSTime::setTime(const char* term)
{
    newTime = (uint32_t)iFlightGPSLib::parseDecimal(term);
}

void iFlightGPSDate::setDate(const char* term)
{
    newDate = atol(term);
}

uint16_t iFlightGPSDate::year()
{
    updated = false;
    uint16_t year = date % 100;
    return year + 2000;
}

uint8_t iFlightGPSDate::month()
{
    updated = false;
    return (date / 100) % 100;
}

uint8_t iFlightGPSDate::day()
{
    updated = false;
    return date / 10000;
}

uint8_t iFlightGPSTime::hour()
{
    updated = false;
    return time / 1000000;
}

uint8_t iFlightGPSTime::minute()
{
    updated = false;
    return (time / 10000) % 100;
}

uint8_t iFlightGPSTime::second()
{
    updated = false;
    return (time / 100) % 100;
}

uint8_t iFlightGPSTime::centisecond()
{
    updated = false;
    return time % 100;
}

void iFlightGPSDecimal::commit()
{
    val = newval;
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSDecimal::set(const char* term)
{
    newval = iFlightGPSLib::parseDecimal(term);
}

void iFlightGPSInteger::commit()
{
    val = newval;
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSInteger::set(const char* term)
{
    newval = atol(term);
}

iFlightGPSLibCustom::iFlightGPSLibCustom(iFlightGPSLib& gps, const char* _sentenceName, int _termNumber)
{
    begin(gps, _sentenceName, _termNumber);
}

void iFlightGPSLibCustom::begin(iFlightGPSLib& gps, const char* _sentenceName, int _termNumber)
{
    lastCommitTime = 0;
    updated = valid = false;
    sentenceName = _sentenceName;
    termNumber = _termNumber;
    memset(stagingBuffer, '\0', sizeof(stagingBuffer));
    memset(buffer, '\0', sizeof(buffer));
    gps.insertCustom(this, _sentenceName, _termNumber);
}

void iFlightGPSLibCustom::commit()
{
    strcpy(this->buffer, this->stagingBuffer);
    lastCommitTime = millis();
    valid = updated = true;
}

void iFlightGPSLibCustom::set(const char* term)
{
    strncpy(this->stagingBuffer, term, sizeof(this->stagingBuffer));
}

void iFlightGPSLib::insertCustom(iFlightGPSLibCustom* pElt, const char* sentenceName, int termNumber)
{
    iFlightGPSLibCustom** ppelt;

    for (ppelt = &this->customElts; *ppelt != NULL; ppelt = &(*ppelt)->next)
    {
        int cmp = strcmp(sentenceName, (*ppelt)->sentenceName);
        if (cmp < 0 || (cmp == 0 && termNumber < (*ppelt)->termNumber))
            break;
    }

    pElt->next = *ppelt;
    *ppelt = pElt;
}