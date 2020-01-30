

#pragma once
#include "main_functions.h"
#include "stdafx.h"
#include "configReader.h"
#include "JFactors.h"
#include "MatrixOfResults.h"
#include "Results.h" 
#include "MyFunctions.h"
#include "WFTransformer.h"
#include "fileNamePrinter.h"
#include "ExtendedRoute.h"



const std::string fileNameLog = "log.txt";
const std::string fileNameRouteNums = "input.txt";


int terminateWithError(bool silentMode, int code, std::string message, std::ofstream& logFile)
{
	logFile << message;
	logFile.close();
	if (!silentMode)
	{
		std::cout << "\n\n\n" << message << "\n\n\n";
		int tmp;
		std::cin >> tmp;
	}
	exit(code);
}


int statesAmount(int nodesAmount)
{
	//singlet levels
	return (int)pow(2, nodesAmount);
	//triplet
	//return 3 * (nodesAmount)*(int)pow((double)2, nodesAmount - 1)	
}

void eval_wavefuns_from_the_beggining(int realSize, 
									  int order,
									  int edgeNum,
									  int nodeNum,
									  WaveFunction wfIn[], 
								      WaveFunction wfOut[],
									  WFTransformer &mainTransformer,
									  int procedureOrder[],
									  int curOperatorSet[],
									  int powerOrder[]) {
	WaveFunction* ref1 = NULL, * ref2 = NULL;
	WaveFunction wfTemp1, wfTemp2;
	for (int ll = 0; ll < realSize; ll++)//��������� ������. ������
	{
		WaveFunction* ref1 = &wfIn[ll];
		ref2 = &wfTemp1;
		for (int mm = 0; mm < (order + 1) / 2; mm++)
		{
			if (mm == (order + 1) / 2 - 1)//���� ������� ��������� ���
				ref2 = &wfOut[ll];

			if (procedureOrder[mm] <= 4)//�������� ���������
				mainTransformer.actPairMatrix(*ref1, *ref2, curOperatorSet[mm], procedureOrder[mm], powerOrder[mm]);
			else
				mainTransformer.actPairInside(*ref1, *ref2, curOperatorSet[mm] - edgeNum, procedureOrder[mm], powerOrder[mm]);

			if (ref2 == &wfTemp1)
			{
				ref2 = &wfTemp2;
				ref1 = &wfTemp1;
				wfTemp2.clear(nodeNum);
			}
			else if (ref2 == &wfTemp2)
			{
				ref2 = &wfTemp1;
				ref1 = &wfTemp2;
				wfTemp1.clear(nodeNum);
			}
		}
	}
}


void eval_wavefuns_from_the_end(int realSize,
								int order,
								int edgeNum,
								int nodeNum,
								WaveFunction wfIn[],
								WaveFunction wfOut[],
								WFTransformer& mainTransformer,
								int procedureOrder[],
								int curOperatorSet[],
								int powerOrder[]) {
	WaveFunction* ref1 = NULL, * ref2 = NULL;
	WaveFunction wfTemp1, wfTemp2;
	for (int ll = 0; ll < realSize; ll++)
	{
		ref1 = &wfIn[ll];
		ref2 = &wfTemp1;
		//��� ������ ������� ������� ������ ��������
		if (order == 1)
		{
			mainTransformer.actCopy(wfIn[ll], wfOut[ll]);
		}
		//��� ���� ��������� ��������
		for (int mm = order - 1; mm > order - 1 - order / 2; mm--)
		{
			if (mm == order - order / 2)//���� ������� ��������� ���
			{
				ref2 = &wfOut[ll];
			}

			if (procedureOrder[mm] <= 4)
			{
				mainTransformer.actPairMatrix(*ref1, *ref2, curOperatorSet[mm], procedureOrder[mm], powerOrder[mm]);
			}
			else
			{
				mainTransformer.actPairInside(*ref1, *ref2, curOperatorSet[mm] - edgeNum, procedureOrder[mm], powerOrder[mm]);
			}

			if (ref2 == &wfTemp1)
			{
				ref2 = &wfTemp2;
				ref1 = &wfTemp1;
				wfTemp2.clear(nodeNum);
			}
			else
				if (ref2 == &wfTemp2)
				{
					ref2 = &wfTemp1;
					ref1 = &wfTemp2;
					wfTemp1.clear(nodeNum);
				}
		}
	}
}


int _main(int argc, char* argv[])
{

	//public for all threads:



	std::ofstream logfile(fileNameLog, std::ios::out);

	std::ifstream terms;//��������� ���� ������ ����������
	std::ifstream operatorsset;//��������� ������ ����������

	bool silentMode = false;
	int startRouteNum, finRouteNum;

	//private:
	std::ofstream matrixRes;  //�������� �����

	std::vector<point> nodes;//������� �������� ��������
	WaveFunction* ref1 = NULL, * ref2 = NULL;

	//check:
	configReader confReader;

	std::stringstream fname;
	std::string fileNameString;
	std::istringstream sscanner;



	int type, order, subOrder; //��� ���������
	int routesAmount[N + 1][RouteTypeAmount];
	std::vector<int*> nodeSets;
	int* termOrder = NULL;
	int edgeNum;//����� ����� � ��������
	int nodeNum;//����� ������ � ��������

	ExtendedRoute curRoute;




	WFTransformer mainTransformer;



	std::string outString;
	std::string strType;
	std::string s; //reading from file

	int* curOperatorSet;
	State init;//� ������� ��� ������
	WaveFunction* wfIn, * wfOut1, * wfOut2;
	WaveFunction wfTemp1, wfTemp2;

	int vecAmount;
	int* procedureOrder;
	int* powerOrder;

	MatrixOfResults fullMatrix;
	int realSize;

	int testMode;
	//Read num of first and the last route.
	confReader.openConfigFile(fileNameRouteNums);
	type = confReader.readIntWithHeader();
	order = confReader.readNextInt();
	subOrder = confReader.readNextInt();
	startRouteNum = confReader.readIntWithHeader();
	finRouteNum = confReader.readNextInt();
	testMode = confReader.readIntWithHeader();
	confReader.closeConfig();



	confReader.openConfigFile(fileNamePrinter::getPathToConfigFile());

	///Error checking
	if (statesAmount(N) > Namount)
		terminateWithError(silentMode, 1, "Change Namount", logfile);
	//Test input data
	if (order > N)
		terminateWithError(silentMode, 3, "\nWrong Const or Config file\n", logfile);
	//Type check
	if (type >= 0 && type < 3)
		strType = StrType[type];
	else
		terminateWithError(silentMode, 2, "Wrong string type!", logfile);

	//Set params for Jfactors
	JFactors::setOrder(order);
	//reading of route amounts of each type
	confReader.readRouteAmounts(routesAmount, 1, N);
	confReader.closeConfig();

	//Init of arrays for numbers of procedure, and powers of denominators in perturbation series
	procedureOrder = new int[order];
	powerOrder = new int[order];




	//��������� ��� ��������� ��� ������� �������
	//�������� ��� ������� �������
	if (order == 1)
	{
		termOrder = new int[1];
		termOrder[0] = -1;
		nodeSets.push_back(termOrder);
	}
	else//��� ���� ���������
	{
		//std::string ttt = fileNamePrinter::getPathToPerturbationTerms(order);
		terms.open(fileNamePrinter::getPathToPerturbationTerms(order), std::ios::in);
		while (!terms.eof())
		{
			getline(terms, s);
			sscanner.clear();
			sscanner.str(s);
			if (s.length() > 0)
			{
				termOrder = new int[order - 1];
				for (int i = 0; i < order - 1; i++)
					sscanner >> termOrder[i];
				nodeSets.push_back(termOrder);
			}
		}
	}

	mainTransformer.pairOperator.readMatrixAndEnergie();

	if (testMode) std::cout << "E0 for 2 nodes: " << mainTransformer.pairOperator.getE0(2) << "\n";

	//Initializing input and output arrays of WF
	vecAmount = (int)pow((double)2, order + 3);
	wfIn = new WaveFunction[vecAmount];
	wfOut1 = new WaveFunction[vecAmount];
	wfOut2 = new WaveFunction[vecAmount];

	//�������������� ������� �����������
	fullMatrix.clearAndSetSize(vecAmount);
	//����� �������������;

	curOperatorSet = new int[order];
	procedureOrder = new int[order];
	powerOrder = new int[order];

	std::vector<Point> routePoints;
	for (int i = subOrder; i <= subOrder; i++)//���������� ��� ��������� ����� ���������
	{
		for (int j = 1; j <= routesAmount[subOrder][type]; j++)//���������� ��� ��������� �������� ��� ������ �����
		{
			//���� ���������� ���������� �������, ��������� ��������� ������ ����� ���������
			if (j > finRouteNum)//���� ��� ���������, �� �������.
			{
				j = 1 + routesAmount[subOrder][type];
				continue;
			}
			if (j < startRouteNum)
				continue;//���������� ���� �� �������� �� �������� �������
			//����� ����� ���������� ���������� �������


			if (DEBUG && false)
			{
				auto ts = fileNamePrinter::getPathToRouteFile(order, i, j, strType);
				std::ifstream temp(ts, std::ios::in);
				getline(temp, ts);
				int i1, i2, i3;
				temp >> i1 >> i2 >> i3;
				std::cout << i1 << " " << i2 << " " << i3 << " \n";
				return 0;
			}

			operatorsset.open(fileNamePrinter::getPathToRouteFile(order, i, j, strType), std::ios::in);
			getline(operatorsset, s);


			operatorsset >> edgeNum >> nodeNum;
			//���� ��� �� ������ ����������� ���������, �� ��������� �����
			if (edgeNum > 0)
			{
				curRoute.readRouteFromString(s);

				curRoute.evaluateRouteProperties(routePoints);

				mainTransformer.clear();
				mainTransformer.setExtendedInteractions(curRoute);
			}

			//��������� ����� ������ ���
			std::ifstream inStates(fileNamePrinter::getPathToSpinsOrder(nodeNum), std::ios::in);

			int curState;

			for (int i = 0; i < vecAmount; i++)
			{
				wfIn[i].clear(nodeNum);
				init.clear();
				for (int j = 0; j < nodeNum; j++)
				{
					inStates >> curState;
					init.addClusterState(curState);
				}
				wfIn[i].addEigenState(init);

			}
			inStates.close();
			//����� ���������� ������



			getline(operatorsset, s);
			realSize = statesAmount(nodeNum);

			fullMatrix.clearAndSetSize(realSize);

			int zz = 0;
			while (!operatorsset.eof())
			{
				getline(operatorsset, s);

				if (s.length() > 0)
				{

					std::cout << order << " " << i << " " << j << " zz=" << zz << "\n";

					zz++;
					sscanner.clear();
					sscanner.str(s);
					for (int k = 0; k < order; k++)
					{
						sscanner >> curOperatorSet[k];//��������� n ���������� ����� ��������
					}

					for (unsigned int k = 0; k < nodeSets.size(); k++)
					{

						for (int ll = 0; ll < realSize; ll++)//������� �������� ������
						{
							wfOut1[ll].clear(nodeNum);
							wfOut2[ll].clear(nodeNum);
						}

						generate_procedure_order(nodeSets[k], curOperatorSet, edgeNum, order, procedureOrder, powerOrder);

						eval_wavefuns_from_the_beggining(realSize, order, edgeNum, nodeNum, wfIn, wfOut1, mainTransformer, procedureOrder, curOperatorSet, powerOrder);

						///��������� ���������� ������

						eval_wavefuns_from_the_end(realSize, order, edgeNum, nodeNum, wfIn, wfOut2, mainTransformer, procedureOrder, curOperatorSet, powerOrder);


						for (int i = 0; i < realSize; i++)
						{
							wfOut1[i].sortWF();
							wfOut2[i].sortWF();
						}

						for (int x = 0; x < realSize; x++)
						{
							for (int y = 0; y < realSize; y++)
							{
								Results tmpres;

								if (DEBUG)
								{
									if (x == 0 && y == 3)
										DEBUGflag = true;
									else
										DEBUGflag = false;
								}

								tmpres.scalarProduct(wfOut1[x], wfOut2[y]);

								if (minus1(nodeSets[k], order) == -1)
									tmpres.minus();

								fullMatrix.add(x, y, tmpres);
							}
						}
					}
				}
			}

			//������ �������
			fullMatrix.printMatrix(fileNamePrinter::getPathToResMatrixFiles(strType, order, i, j), order);
			operatorsset.close();
		}
	}

	//finishing
	delete[] curOperatorSet;
	for (auto& elem : nodeSets)
		delete[] elem;

	//if (termOrder != NULL) delete[] termOrder;
	delete[] wfIn;
	delete[] wfOut1;
	delete[] wfOut2;

	delete[] procedureOrder;
	delete[] powerOrder;

	logfile.close();
	if (testMode)
	{
		std::cout << "Cin to exit: ";
		std::cin >> vecAmount;
	}
	return 0;
}