/* ORIGINAL LICENSE: Do What The F*ck You Want To Public License
 * AUTHOR: LBYPatrick
 *
 * MODIFIED BY Leroy.H.Y @lhy0403 re-licenced under GPLv3
 */

#include "QvUtils.hpp"

namespace Qv2ray
{
    namespace Utils
    {
        // Private function
        string getRawDomain(string originLine)
        {
            size_t startPosition = 0;
            size_t endPosition = originLine.size();
            string returnBuffer;
            bool skipRule1 = originLine.find("[") != string::npos; // [Auto xxxx...
            bool skipRule2 = originLine.find("!") != string::npos; // Comments
            bool skipRule3 = originLine.find("@") != string::npos; // Non-proxy Lines
            bool skipRule4 = originLine.find("*") != string::npos;
            bool passRule1 = originLine.find("|") != string::npos; // Proxy Lines
            bool passRule2 = originLine.find(".") != string::npos; // Link-Contained Lines

            if (originLine[endPosition] == '\n') {
                endPosition -= 1;
            }

            if (originLine.find("http://") != string::npos) {
                startPosition += 8;
            } else if (originLine.find("https://") != string::npos) {
                startPosition += 9;
            }

            // Skip unrelated lines
            if (skipRule1 || skipRule2 || skipRule3 || skipRule4) {
                return "";
            } else if (passRule2) {
                if (passRule1) {
                    startPosition += originLine.find_last_of("|") + 1;
                }

                if (originLine[startPosition] == '\n') startPosition += 1;

                for (size_t i = startPosition; i < endPosition; ++i) {
                    returnBuffer += originLine[i];
                }
            }

            return returnBuffer;
        }

        QString ConvertGFWToPAC(const QString &rawContent, const QString &customProxyString)
        {
            auto rawFileContent = Base64Decode(rawContent).toStdString();
            string readBuffer = ""; //cleanup
            string writeBuffer;
            string domainListCache = "";

            for (size_t i = 0; i < rawFileContent.size(); ++i) {
                readBuffer += rawFileContent[i];

                if (rawFileContent[i + 1] == '\n') {
                    writeBuffer = getRawDomain(readBuffer);

                    if (writeBuffer != "") {
                        domainListCache += writeBuffer + "\n";
                    }

                    readBuffer = "";
                    i += 1;
                }
            }

            size_t rotatorTwo = 0;
            string readDomainBuffer	= "";
            bool isFirstLine = true;
            string outputContent = "";
            //Header
            outputContent += "var domains = {\n";

            //Read and process output content line by line
            while (rotatorTwo < domainListCache.size()) {
                while (true) {
                    //Get Domain
                    readDomainBuffer += domainListCache[rotatorTwo];

                    if (domainListCache[rotatorTwo + 1] == '\n') {
                        rotatorTwo += 2;
                        break;
                    }

                    rotatorTwo++;
                }

                //Format
                if (!isFirstLine) outputContent += ",\n";
                else			  isFirstLine = false;

                outputContent += "\t\"";
                outputContent += readDomainBuffer;
                outputContent += "\" : 1";
                readDomainBuffer = "";
            }

            //End Message
            outputContent += "\n\n};\n\n\n";
            outputContent += "var proxy = \"";
            outputContent += customProxyString.toStdString();
            outputContent += "\";\n";
            outputContent += "var direct = 'DIRECT;';\n";
            outputContent += "var hasOwnProperty = Object.hasOwnProperty;\n\n";
            outputContent += "function FindProxyForURL(url, host) {\n\n";
            outputContent += "\tvar suffix;\n";
            outputContent += "\tvar pos = host.lastIndexOf('.');\n";
            outputContent += "\tpos = host.lastIndexOf('.', pos - 1);\n\n";
            outputContent += "\twhile(1) {\n";
            outputContent += "\t\tif (pos <= 0) {\n";
            outputContent += "\t\t\tif (hasOwnProperty.call(domains, host)) ";
            outputContent += "return proxy;\n";
            outputContent += "\t\t\telse ";
            outputContent += "return direct;\n";
            outputContent += "\t\t}\n\n";
            outputContent += "\tsuffix = host.substring(pos + 1);\n";
            outputContent += "\tif (hasOwnProperty.call(domains, suffix))";
            outputContent += "return proxy;\n";
            outputContent += "\tpos = host.lastIndexOf('.', pos - 1);\n\t}\n}";
            return QSTRING(outputContent);
        }
    }
}
