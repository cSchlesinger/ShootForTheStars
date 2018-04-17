#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, 0.0f), //Position
		vector3(0.0f, 5.0f, 5.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	// Create a floor under the player
	m_pEntityMngr->AddEntity("Sorted\\1x1Plane.obj", "Floor");
	matrix4 m4Position = glm::translate(IDENTITY_M4, vector3(0, -2.1f, 0)) * ToMatrix4(glm::angleAxis(-90.0f, AXIS_X)) * glm::scale(vector3(1000.0f, 1000.0f, 1.0f));
	m_pEntityMngr->SetModelMatrix(m4Position);

	// Generate Stars in a random half sphere around the player
	for (int i = 0; i < 200; i++)
	{
		m_pEntityMngr->AddEntity("Planets\\00_Sun.obj", "Star_" + std::to_string(i));

		// Generate a random sphere position
		vector3 v3Position = vector3(glm::sphericalRand(100.0f));

		// If the sphere point is under the floor move it above the floor by an equal distance
		if (v3Position.y < 0.0f) 
		{
			v3Position.y *= -1;
		}

		// If the sphere point is lower than a minimum amount relocate them further upwards
		if (v3Position.y < 10.0f)
		{
			v3Position.y = 10.0f;
		}

		// Move the star to the calculated position
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		//m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(i+1);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Update Bullets


	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}
void Application::ShootBullet(void)
{
	//Create a new Bullet Entity
	m_pEntityMngr->AddEntity("Sorted\\PolyOut.obj", "Bullet_" + std::to_string(bulletCount));

	//Move the bullet to the camera's current position
	vector3 v3Position = m_pCameraMngr->GetPosition();
	matrix4 m4Position = glm::translate(IDENTITY_M4, v3Position);
	//Move and Rotate the Bullet to match the camera(Rotation does not work with solver)
	//matrix4 m4Position = glm::translate(IDENTITY_M4, v3Position) * ToMatrix4(glm::angleAxis(90.0f, glm::normalize(m_pCameraMngr->GetForward())));
	m_pEntityMngr->SetModelMatrix(m4Position);

	//Have the Bullet use a physics solver
	m_pEntityMngr->UsePhysicsSolver();
	m_pEntityMngr->SetMass(1);
	m_pEntityMngr->SetFriction(0.01f);

	//Have the bullet shoot out away from the player
	vector3 force = glm::normalize(m_pCameraMngr->GetForward()) * 10.0f;
	m_pEntityMngr->GetEntity()->GetEntity("Bullet_" + std::to_string(bulletCount))->GetSolver()->ApplyForce(force);

	bulletCount++;
}