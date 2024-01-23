using System;
using System.Runtime.CompilerServices;

namespace OpenEngine
{

    public class Entity
    {
        protected Entity() { ID = 0; }

        internal Entity(ulong id)
        {
           ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(ID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(ID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(ID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(ID, ref value);
            }
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, type);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            return new T() { Entity = this };
        }

        public Entity GetEntityByName(string name)
        {
            ulong entityID = InternalCalls.Entity_GetEntityByName(name);
            
            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }

    }

}